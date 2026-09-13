#include "core/llm_client.h"

#include <curl/curl.h>

#include <nlohmann/json.hpp>

#include <stdexcept>
#include <string>
#include <utility>

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

LLMClient::LLMClient(
	const std::string endpoint,
	const std::string model) :
	endpoint_(std::move(endpoint)),
	model_(std::move(model))
{}

std::string LLMClient::chat(
	const std::string& prompt)
{
	CURL* curl = curl_easy_init();

	if (!curl)
	{
		throw std::runtime_error(
			"Failed to initialize CURL"
		);
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

	struct curl_slist* headers = nullptr;

	headers = curl_slist_append(
		headers,
		"Content-type: application/json"
	);

	curl_easy_setopt(
		curl,
		CURLOPT_URL,
		endpoint_.c_str()
	);

	curl_easy_setopt(
		curl,
		CURLOPT_POST,
		1L
	);

	curl_easy_setopt(
		curl,
		CURLOPT_POSTFIELDS,
		request_body.c_str()
	);

	curl_easy_setopt(
		curl,
		CURLOPT_HTTPHEADER,
		headers
	);

	curl_easy_setopt(
		curl,
		CURLOPT_WRITEFUNCTION,
		write_callback
	);

	curl_easy_setopt(
		curl,
		CURLOPT_WRITEDATA,
		&response
	);

	curl_easy_setopt(
		curl,
		CURLOPT_TIMEOUT,
		500L
	);

	const CURLcode result =
		curl_easy_perform(curl);

	if (result != CURLE_OK)
	{
		const std::string error =
			curl_easy_strerror(result);

		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);

		throw std::runtime_error(
			"HTTP request fail: " + error
		);
	}

	long http_status = 0;

	curl_easy_getinfo(
		curl,
		CURLINFO_RESPONSE_CODE,
		&http_status
	);

	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

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