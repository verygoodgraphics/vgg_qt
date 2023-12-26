#include "Counter.h"

#include "nlohmann/json.hpp"

#include <string>

Counter::Counter(const std::string &vggFilePath, bool isJsCounter, QWidget* parent) {
  m_vggContainer.show();

  m_vggContainer.load(vggFilePath);

  if (isJsCounter) {
    return;
  }

  m_vggContainer.setEventListener([](std::shared_ptr<VGG::ISdk> vggSdk,
                                     std::string type, std::string targetId,
                                     std::string targetPath) {
    if (targetId == "#counterButton" || targetId == "#counterButtonText") {
      auto buttonPath = "/frames/0/childObjects/1/style/fills/0/color/alpha";

      if (type == "mousedown") {
        nlohmann::json alpha = 1.0;
        vggSdk->designDocumentReplaceAt(buttonPath, alpha.dump());
      } else if (type == "mouseup") {
        nlohmann::json alpha = 0.5;
        vggSdk->designDocumentReplaceAt(buttonPath, alpha.dump());

        // get last value
        auto valuePath = "/frames/0/childObjects/3/content";
        auto value = vggSdk->designDocumentValueAt(valuePath);

        auto jsonValue = nlohmann::json::parse(value);
        auto count = std::stoi(jsonValue.get<std::string>());
        ++count;
        jsonValue = std::to_string(count);

        vggSdk->designDocumentReplaceAt(valuePath, jsonValue.dump());
      }
    }
  });
}
