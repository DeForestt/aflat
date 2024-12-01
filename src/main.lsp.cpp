#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <iostream>
#include <string>
#include <unordered_map>

struct LSPMessage {
  std::unordered_map<std::string, std::string> headers;
  rapidjson::Document body;

  LSPMessage() = default;
  LSPMessage(std::unordered_map<std::string, std::string>&& headers,
             rapidjson::Document&& body)
      : headers(std::move(headers)), body(std::move(body)) {}
};

struct Capabilities {
  bool hoverProvider = false;
  bool definitionProvider = false;
  bool typeDefinitionProvider = false;
  bool implementationProvider = false;
  bool referencesProvider = false;
  bool documentSymbolProvider = false;
  bool workspaceSymbolProvider = false;
  bool codeActionProvider = false;
  bool codeLensProvider = false;
  bool documentFormattingProvider = false;
  bool documentRangeFormattingProvider = false;
  bool documentOnTypeFormattingProvider = false;
  bool renameProvider = false;
  bool documentLinkProvider = false;
  bool colorProvider = false;
  bool foldingRangeProvider = false;
  bool executeCommandProvider = false;
  bool workspace = false;
  bool textDocument = false;
  bool window = false;
  bool experimental = false;

  rapidjson::Document ToJSON() const {
    rapidjson::Document doc;
    doc.SetObject();
    doc.AddMember("hoverProvider", hoverProvider, doc.GetAllocator());
    doc.AddMember("definitionProvider", definitionProvider, doc.GetAllocator());
    doc.AddMember("typeDefinitionProvider", typeDefinitionProvider,
                  doc.GetAllocator());
    doc.AddMember("implementationProvider", implementationProvider,
                  doc.GetAllocator());
    doc.AddMember("referencesProvider", referencesProvider, doc.GetAllocator());
    doc.AddMember("documentSymbolProvider", documentSymbolProvider,
                  doc.GetAllocator());
    doc.AddMember("workspaceSymbolProvider", workspaceSymbolProvider,
                  doc.GetAllocator());
    doc.AddMember("codeActionProvider", codeActionProvider, doc.GetAllocator());
    doc.AddMember("codeLensProvider", codeLensProvider, doc.GetAllocator());
    doc.AddMember("documentFormattingProvider", documentFormattingProvider,
                  doc.GetAllocator());
    doc.AddMember("documentRangeFormattingProvider",
                  documentRangeFormattingProvider, doc.GetAllocator());
    doc.AddMember("documentOnTypeFormattingProvider",
                  documentOnTypeFormattingProvider, doc.GetAllocator());
    doc.AddMember("renameProvider", renameProvider, doc.GetAllocator());
    doc.AddMember("documentLinkProvider", documentLinkProvider,
                  doc.GetAllocator());
    doc.AddMember("colorProvider", colorProvider, doc.GetAllocator());
    doc.AddMember("foldingRangeProvider", foldingRangeProvider,
                  doc.GetAllocator());
    doc.AddMember("executeCommandProvider", executeCommandProvider,
                  doc.GetAllocator());
    doc.AddMember("workspace", workspace, doc.GetAllocator());
    doc.AddMember("textDocument", textDocument, doc.GetAllocator());
    doc.AddMember("window", window, doc.GetAllocator());
    doc.AddMember("experimental", experimental, doc.GetAllocator());
    return doc;
  }
}

class LanguageServer {
 public:
  LanguageServer() = default;
  ~LanguageServer() = default;

  void Run() {
    while (true) {
      LSPMessage message = ReadMessage();
      ProcessMessage(message);
      return;  // return for testing...
    }
  }

 private:
  bool initialized = false;

  void ProcessMessage(const LSPMessage& message) {
    // echo for testing...
  }

#pragma region LSP Methods
  LSPMessage Initialize(const LSPMessage& message) {
    initialized = true;

    rapidjson::Document response;
    response.SetObject();
    response.AddMember("capabilities", Capabilities().ToJSON(),
                       response.GetAllocator());
    return LSPMessage(message.headers, std::move(response));
  }

  LSPMessage ReadMessage() {
    std::string header_line;
    std::getline(std::cin, header_line);

    auto headers = std::unordered_map<std::string, std::string>();
    while (header_line != "\r") {
      size_t colon_pos = header_line.find(':');
      if (colon_pos == std::string::npos) {
        return LSPMessage();
      }

      std::string key = header_line.substr(0, colon_pos);
      std::string value = header_line.substr(colon_pos + 2);  // skip ": "
      headers[key] = value;

      std::getline(std::cin, header_line);
    }

    if (headers.find("Content-Length") == headers.end()) {
      return LSPMessage();
    }

    int content_length = std::stoi(headers["Content-Length"]);
    std::string body(content_length, '\0');
    std::cin.read(&body[0], content_length);

    rapidjson::Document doc;
    doc.Parse(body.c_str());
    if (doc.HasParseError()) {
      std::cerr << "Failed to parse JSON body" << std::endl;
      return LSPMessage();
    }

    if (!doc.IsObject()) {
      std::cerr << "Invalid JSON body" << std::endl;
      return LSPMessage();
    }

    return LSPMessage(std::move(headers), std::move(doc));
  };

  void SendResponse(const LSPMessage& message) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    message.body.Accept(writer);
    std::string response =
        "Content-Length: " + std::to_string(buffer.GetSize()) + "\r\n\r\n";
    std::cout << response << buffer.GetString() << std::endl;
  }
};

int main() {
  LanguageServer server;
  server.Run();
  return 0;
}
