#pragma once
#include <string>
#include "Json.hpp"
#include <locale>
#include <codecvt>
#include <Windows.h>
#include <winhttp.h>
#pragma comment (lib, "winhttp.lib")

template <typename T>
struct Property {
protected:
	T _value;
public:
	Property() : _value(T()) {}
	Property(const T& value) : _value(value) {}
	Property(const Property<T>& other) : _value(other._value) {}

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
	Embed& addField(const nlohmann::json& json_data) {
		_data["fields"].push_back(json_data);
		return *this;
	}
	//{ "name": "", "value":"value", "inline":bool } の配列
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
public:
	Message(const std::string& content = "") {
		_data["content"] = content;
	};
	~Message() {};
};

class Discord final {
public: //関数の引数として使う場合は明示的なキャストが必要です
	Property<std::string> avatar;
	Property<std::string> username;
	Property<std::string> webhook;
	Property<HINTERNET> client;
	Property<HINTERNET> connect;
public:
	static std::wstring strToWstr(const std::string& str) {
		int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
		std::wstring result(size, 0);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size);
		return result;
	}
public:
	bool sendWebhook(const Message& message) {
		auto dumped = message.string();

		auto request = WinHttpOpenRequest(
			connect,
			L"POST",
			strToWstr(webhook).c_str(),
			NULL,
			WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			WINHTTP_FLAG_SECURE
		);

		auto res = WinHttpSendRequest(
			request,
			L"Content-Type: application/json",
			-1,
			(LPVOID)dumped.c_str(),
			dumped.length(),
			dumped.length(),
			0
		);

		WinHttpReceiveResponse(request, NULL);
		return res;
	}

public:
	Discord(const std::string& webhook_url) : webhook(webhook_url) {
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
		WinHttpCloseHandle(client);
	}
};