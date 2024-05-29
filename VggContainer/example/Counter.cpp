#include "Counter.h"

#include "nlohmann/json.hpp"

#include <string>

Counter::Counter(const std::string& vggFilePath, bool isJsCounter, QWidget* parent)
{
  m_vggContainer.show();
  m_vggContainer.load(vggFilePath);

  if (isJsCounter)
  {
    return;
  }

  m_vggContainer.setEventListener(
    [](
      std::shared_ptr<VGG::ISdk> vggSdk,
      std::string                type,
      std::string                targetId,
      std::string                targetName)
    {
      if (targetName == "#counterButton" || targetName == "#counterButtonText")
      {

        if (type == "mousedown")
        {
          // change button background color alpha
          {
            const auto  id = "#counterButton";
            const auto& e = vggSdk->getElement(id);
            auto        j = nlohmann::json::parse(e);
            j["style"]["fills"][0]["color"]["alpha"] = 1.0;
            vggSdk->updateElement(id, j.dump());
          }
        }
        else if (type == "mouseup")
        {
          // change button background color alpha
          {
            const auto  id = "#counterButton";
            const auto& e = vggSdk->getElement(id);
            auto        j = nlohmann::json::parse(e);
            j["style"]["fills"][0]["color"]["alpha"] = 0.5;
            vggSdk->updateElement(id, j.dump());
          }

          // update count number
          {
            // get count number
            const auto  id = "#count";
            const auto& e = vggSdk->getElement(id);
            const auto& j = nlohmann::json::parse(e);
            auto        count = std::stoi(j["content"].get<std::string>());

            // increase
            ++count;

            // update count text
            nlohmann::json patch;
            patch["content"] = std::to_string(count);
            vggSdk->updateElement(id, patch.dump());
          }
        }
      }
    });
}
