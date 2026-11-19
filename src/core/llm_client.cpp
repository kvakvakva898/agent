#include "core/llm_client.h"

#include <curl/curl.h>

#include <nlohmann/json.hpp>

#include <stdexcept>
#include <string>
#include <utility>
#include <memory>
#include <atomic>

using json = nlohmann::json;

static size_t write_callback(
	char* data,
	size_t size,
	size_t count,
	void* user_data)
{
	const size_t total_size = size * count;

	auto* response = static_cast<std::string*>(user_data);

	response->append(data, total_size);

	return total_size;
}

static int progress_callback(
	void* client_data,
	curl_off_t download_total,
	curl_off_t downloaded,
	curl_off_t upload_total,
	curl_off_t uploaded)
{
	auto* cancel_requested =
		static_cast<const std::atomic_bool*>(client_data);

	if (!cancel_requested)
		return 0;

	return cancel_requested->load(
		std::memory_order_relaxed
	) ? 1 : 0;
}

LLMClient::LLMClient(
	const std::string endpoint,
	const std::string model) :
	endpoint_(std::move(endpoint)),
	model_(std::move(model))
{}

std::string LLMClient::chat(
	const std::string& prompt,
	const std::atomic_bool* cancel_requested_)
{
	if (cancel_requested_ &&
		cancel_requested_->load(std::memory_order_relaxed))
	{
		throw RequestCancelled();
	}

	using CurlHandle =
		std::unique_ptr<CURL, decltype(&curl_easy_cleanup)>;

	CurlHandle curl(
		curl_easy_init(),
		&curl_easy_cleanup);

	if (!curl)
	{
		throw std::runtime_error(
			"Failed to initialize CURL");
	}

	std::string response;

	json request;

	request["model"] = model_;

	request["messages"] = json::array();

	request["messages"].push_back({
		{"role", "user"},
		{"content", prompt}
		});

	request["temperature"] = 0.2;

	const std::string request_body = request.dump();

	struct CurlSlistDeleter
	{
		void operator()(curl_slist* list) const
		{
			curl_slist_free_all(list);
		}
	};

	std::unique_ptr<curl_slist, CurlSlistDeleter> headers(
		curl_slist_append(
			nullptr,
			"Content-type: application/json"));

	if (!headers)
	{
		throw std::runtime_error(
			"Failed to allocate CURL headers");
	}

	curl_easy_setopt(
		curl.get(),
		CURLOPT_URL,
		endpoint_.c_str()
	);

	curl_easy_setopt(
		curl.get(),
		CURLOPT_POST,
		1L
	);

	curl_easy_setopt(
		curl.get(),
		CURLOPT_POSTFIELDS,
		request_body.c_str()
	);

	curl_easy_setopt(
		curl.get(),
		CURLOPT_HTTPHEADER,
		headers.get()
	);

	curl_easy_setopt(
		curl.get(),
		CURLOPT_WRITEFUNCTION,
		write_callback
	);

	curl_easy_setopt(
		curl.get(),
		CURLOPT_WRITEDATA,
		&response
	);

	curl_easy_setopt(
		curl.get(),
		CURLOPT_TIMEOUT,
		500L
	);

	curl_easy_setopt(
		curl.get(),
		CURLOPT_NOSIGNAL,
		1L
	);

	curl_easy_setopt(
		curl.get(),
		CURLOPT_NOPROGRESS,
		0L
	);

	curl_easy_setopt(
		curl.get(),
		CURLOPT_XFERINFOFUNCTION,
		progress_callback
	);

	curl_easy_setopt(
		curl.get(),
		CURLOPT_XFERINFODATA,
		cancel_requested_
	);

	const CURLcode result =
		curl_easy_perform(curl.get());

	if (result == CURLE_ABORTED_BY_CALLBACK)
	{
		throw RequestCancelled();
	}

	if (result != CURLE_OK)
	{
		throw std::runtime_error(
			std::string("HTTP request failed: ")
			+ curl_easy_strerror(result)
		);
	}

	long http_status = 0;

	curl_easy_getinfo(
		curl.get(),
		CURLINFO_RESPONSE_CODE,
		&http_status
	);

	if (http_status < 200 || http_status >= 300)
	{
		throw std::runtime_error(
			std::string("Server returned HTTP status: ")
			+ std::to_string(http_status)
			+ "\nResponse: "
			+ response
		);
	}

	try
	{
		const json response_json =
			json::parse(response);

		return response_json
			.at("choices")
			.at(0)
			.at("message")
			.at("content")
			.get<std::string>();
	}
	catch (const json::exception& e)
	{
		throw std::runtime_error(
			std::string("Invalid LLM response: ")
			+ e.what()
			+ "\nResponse: "
			+ response
		);
	}
}