#include <type_traits>
#include <painlessMeshPlugins/EspMeshDevice.hpp>
#include <Modes/LightMode.hpp>
#include <esp32_digital_led_lib.h>
#include <LittleFSWrapper.hpp>

class LedStrip : public EspMeshDevice
{
public:
    virtual void loop() override;

    // virtual void setup(String type, uint32_t firmwareVersion, bool enableRootConnectedCheck);

    struct LedStripValue
    {
        char mode[32];
        int delay;
        int numberOfLeds;
        int brightness;
        uint32_t step;
        bool reverse;
        pixelColor_t pixelColor;
        uint16_t version;

        friend bool operator==(const LedStripValue &c1, const LedStripValue &c2)
        {
            return strcmp(c1.mode, c2.mode) && c1.delay == c2.delay && c1.numberOfLeds == c2.numberOfLeds && c1.brightness == c2.brightness && c1.step == c2.step && c1.reverse == c2.reverse && c1.pixelColor.num == c2.pixelColor.num && c1.version == c2.version;
        }

        friend bool operator!=(const LedStripValue &c1, const LedStripValue &c2)
        {
            return !(c1 == c2);
        }

        void setMode(const std::string &newMode)
        {
            std::string temp(newMode);
            if (temp.size() < sizeof(mode))
                temp.append(32 - temp.size(), '\0');
            strncpy(mode, temp.c_str(), sizeof(mode));
        }
    };

protected:
    virtual void OnMeshMsgReceived(uint32_t from, const std::string &messageType, const std::string &command, const std::vector<MessageParameter> &parameter) override;
    virtual void restartMesh() override;
    virtual std::vector<MessageParameter> AdditionalWhoAmIResponseParams() override;
    virtual void preMeshSetup();
    virtual void preReboot() override;

private:
    template <class T, typename = typename std::enable_if<std::is_base_of<LEDLightMode, T>::value>::type>
    void CreateTask(int delay, bool sendToServer = true);
    void saveCurrentState();

    template <class T>
    void printDebug(String name, T value)
    {
        Serial.print(name);
        Serial.println(value);
    }

    std::unique_ptr<Task> ledLoopTask;
    std::unique_ptr<LEDLightMode> mode;
    LedStripValue runtimeValue;
    strand_t pStrand;
    bool manualInputPin;
    bool oldPinState;
    int lastTime;
};