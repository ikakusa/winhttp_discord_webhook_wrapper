#pragma once
#include <string>
#include "Json.hpp"
#include <vector>
#include <Windows.h>
#include <winhttp.h>
#include <chrono>
#include <sstream>
#pragma comment (lib, "winhttp.lib")

template <typename T>
struct Property {
protected:
	T _value;
public:
	Property() : _value(T()) {}
	Property(const T& value) : _value(value) {}
	Property(const Property<T>& other) : _value(other._value) {}

	void set(const T& other_v) {
		this->_value = other_v;
	}

	void set(const Property<T>& other) {
		this->_value = other.get();
	}

	const T& get() const {
		return _value;
	}

	Property<T>& operator=(const Property<T>& other) {
		this->_value = other._value;
		return *this;
	}
	operator const T& () const {
		return _value;
	}
	T operator ->() const {
		return _value;
	}
};

struct MessageBaseType {
protected:
	nlohmann::json _data;
public:
	nlohmann::json json() const {
		return _data;
	}
	std::string string() const {
		return _data.dump();
	}
};

struct Embed : public MessageBaseType {
public:
	Embed& setTitle(const std::string& title) {
		_data["title"] = title;
		return *this;
	}
	Embed& setDescription(const std::string & desc) {
		_data["description"] = desc;
		return *this;
	}
	Embed& setURL(const std::string& url) {
		_data["url"] = url;
		return *this;
	}
	Embed& setColor(std::string hex_color) {
		if (hex_color[0] == '#') hex_color.erase(0, 1);
		_data["color"] = std::stoi(hex_color, nullptr, 16);
		return *this;
	}
	Embed& setColor(int color) {
		_data["color"] = color;
		return *this;
	}
	Embed& setThumbnail(const std::string& url) {
		_data["thumbnail"]["url"] = url;
		return *this;
	}
	Embed& setImage(const std::string& url) {
		_data["image"]["url"] = url;
		return *this;
	}
	Embed& setFooter(const std::string& text, const std::string& icon_url = "") {
		_data["footer"] = { {"text", text}, {"icon_url", icon_url} };
		return *this;
	}
	Embed& setAuthor(const std::string& text, const std::string& icon_url = "", const std::string& url = "") {
		_data["author"] = { {"name", text}, { "url", url}, {"icon_url", icon_url}};
		return *this;
	}
	Embed& addField(const std::string& name, const std::string& value, bool set_inline = false) {
		_data["fields"].push_back(
			{
				{ "name", name },
				{ "value", value },
				{ "inline", set_inline }
			}
		);
		return *this;
	}
	Embed& setTimestamp(std::chrono::system_clock::time_point time = std::chrono::system_clock::now()) {
		std::time_t t = std::chrono::system_clock::to_time_t(time);
		std::tm tm{};
		gmtime_s(&tm, &t); // UTC
		std::ostringstream oss;
		oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
		_data["timestamp"] = oss.str();
		return *this;
	};
	Embed& addField(const nlohmann::json& json_data) {
		_data["fields"].push_back(json_data);
		return *this;
	}
	//{ "name": "", "value":"value", "inline":bool } array
	Embed& setField(const nlohmann::json& json_data) {
		_data["fields"] = json_data;
		return *this;
	}
	nlohmann::json json() const {
		return _data;
	}
	Embed() {};
	~Embed() {};
};

struct EmbedBuilder : public MessageBaseType {
public:
	EmbedBuilder& addEmbed(const Embed& embed) {
		_data.push_back(embed.json());
		return *this;
	}
public:
	EmbedBuilder() {};
	~EmbedBuilder() {};
};

struct Poll : public MessageBaseType {
private:
	int answer_id = 1;
public:
	[[deprecated]]
	Poll& setLayoutType(int type = 1) {
		_data["layout_type"] = type;
		return *this;
	}
	Poll& setDuration(int duration) {
		_data["duration"] = duration;
		return *this;
	}
	Poll& setQuestion(const std::string& text) {
		_data["question"] = text;
		return *this;
	}
	//TODO: emoji
	Poll& addAnswer(const std::string& text, const std::string& emoji = "", const std::string& emoji_id = "") {
		nlohmann::json answer_obj = {};
		answer_obj["poll_media"]["text"] = text;
		answer_obj["answer_id"] = answer_id;
		/*TODO
		if (!emoji.empty()) {
			answer_obj["poll_media"]["emoji"]["name"] = emoji;
			if (!emoji_id.empty()) answer_obj["poll_media"]["emoji"]["id"] = emoji_id;
		}
		*/
		_data["answers"].push_back(answer_obj);
		++answer_id;
		return *this;
	}
	Poll& setAllowMultiselect(bool allow) {
		_data["allow_multiselect"] = allow;
		return *this;
	}
public:
	//duration = hour
	Poll(const std::string& question, bool allow_multiselect = false, int duration = 24) {
		_data["allow_multiselect"] = allow_multiselect;
		_data["question"]["text"] = question;
		_data["duration"] = duration;
		_data["layout_type"] = 1;
	};
	~Poll() {};
};

struct Message : public MessageBaseType {
public:
	[[deprecated]]
	Message& content(const std::string& content) {
		_data["content"] = content;
		return *this;
	}
	Message& embed(const Embed& embed) {
		_data["embeds"].push_back(embed.json());
		return *this;
	}
	Message& addEmbeds(const EmbedBuilder& embed) {
		for (auto& emb : embed.json().get<nlohmann::json::array_t>()) {
			_data["embeds"].push_back(emb);
		}
		return *this;
	}
	Message& setAllowedMentions(bool allowed) {
		_data["allowed_mentions"] = false;
		return *this;
	}
	Message& addPoll(const Poll& poll) {
		_data["poll"] = poll.json();
		return *this;
	}
public:
	Message(const std::string& content = "") {
		_data["content"] = content;
	};
	~Message() {};
};

struct DiscordFormData {
private:
	static inline std::string _boundary = "formdataboundary";
	std::string _raw;
	int _file_count = 0;
public:
	DiscordFormData& add_payload(nlohmann::json json) {
		_raw += "--" + _boundary + "\r\n";
		_raw += "Content-Disposition: form-data; name=\"payload_json\"\r\n";
		_raw += "Content-Type: application/json\r\n\r\n";
		_raw += json.dump() + "\r\n";
		return *this;
	}
	DiscordFormData& add_file(const std::string& filename, const std::string& content_type, std::vector<char> file_bytes) {
		_raw += "--" + _boundary + "\r\n";
		_raw += "Content-Disposition: form-data; name=\"files[" + std::to_string(_file_count) + "]" + "\"; ";
		_raw += "filename=\"" + filename + "\"";
		_raw += "\r\n";
		_raw += "Content-Type:" + content_type + "\r\n\r\n";
		_raw.append(file_bytes.data(), file_bytes.size());
		_raw += "\r\n";
		++_file_count;
		return *this;
	}
	static std::string boundary() {
		return _boundary;
	}
	std::string string() const {
		return _raw + "--" + _boundary + "--\r\n";
	}
	DiscordFormData() {};
};

class Discord final {
public:
	Property<std::string> avatar;
	Property<std::string> username;
private:
	Property<std::string> webhook;
	Property<HINTERNET> client;
	Property<HINTERNET> connect;
	Property<HINTERNET> request;
public:
	static std::wstring strToWstr(const std::string& str) {
		int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
		std::wstring result(size, 0);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size);
		return result;
	}
public:
	bool sendWebhook(const Message& message, DiscordFormData data) {
		auto msg = message.json();
		msg["username"] = username.get();
		msg["avatar_url"] = avatar.get();
		data.add_payload(msg);

		if (!request) {
			request = WinHttpOpenRequest(
				connect,
				L"POST",
				strToWstr(webhook).c_str(),
				NULL,
				WINHTTP_NO_REFERER,
				WINHTTP_DEFAULT_ACCEPT_TYPES,
				WINHTTP_FLAG_SECURE
			);
		}

		auto body = data.string();
		auto res = WinHttpSendRequest(
			request,
			(L"Content-Type: multipart/form-data; boundary=" + strToWstr(DiscordFormData::boundary())).c_str(),
			-1,
			(LPVOID)body.c_str(),
			body.length(),
			body.length(),
			0
		);

		WinHttpReceiveResponse(request, NULL);
		return res;
	}

	void setWebhook(std::string webhook_url) {
		if (webhook_url._Starts_with("https://")) webhook_url.erase(0, 8);
		if (webhook_url._Starts_with("discord.com")) webhook_url.erase(0, 11);
		webhook = webhook_url;
		if (request) WinHttpCloseHandle(request);
		request = WinHttpOpenRequest(
			connect,
			L"POST",
			strToWstr(webhook).c_str(),
			NULL,
			WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			WINHTTP_FLAG_SECURE
		);
	}
public:
	Discord(std::string webhook_url) {
		if (webhook_url._Starts_with("https://")) webhook_url.erase(0, 8);
		if (webhook_url._Starts_with("discord.com")) webhook_url.erase(0, 11);
		webhook = webhook_url;
		client = WinHttpOpen(
			L"UserAgent/WinHTTP 1.0",
			WINHTTP_ACCESS_TYPE_NO_PROXY,
			WINHTTP_NO_PROXY_NAME,
			WINHTTP_NO_PROXY_BYPASS,
			0
		);
		
		connect = WinHttpConnect(
			client,
			L"discord.com",
			INTERNET_DEFAULT_HTTPS_PORT,
			0
		);
	};

	~Discord() {
		if (client) WinHttpCloseHandle(client);
		if (connect) WinHttpCloseHandle(connect);
		if (request) WinHttpCloseHandle(request);
	}
};