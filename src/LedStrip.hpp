#include <type_traits>
#include <Device.h>
#include <Modes/LightMode.hpp>
#include <esp32_digital_led_lib.h>

class LedStrip : public Device
{

private:

public:
    LedStrip() : Device("ledstrip", "V2.1"){}
    virtual void init() override;
    virtual void loop() override;


protected:
    virtual void OnMsgReceived(uint32_t from, const String &messageType, const String &command, const String &parameter) override;
    virtual std::vector<MessageParameter> AdditionalWhoAmIResponseParams() override;

private:
    template<class T, typename = typename std::enable_if<std::is_base_of<LEDLightMode, T>::value>::type>
    void CreateTask(int delay);

    std::unique_ptr<Task> ledLoopTask;
    std::unique_ptr<LEDLightMode> mode;
    pixelColor_t pixelColor;
    int delay;
    int numberOfLeds;
    int step;
    int brightness;
    bool reverse;
    strand_t pStrand;
    String lastUpdateCommand;
    bool manualInputPin;
    bool oldPinState;
    int lastTime;
};