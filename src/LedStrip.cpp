#include <LedStrip.hpp>
#include <Modes/Modes.hpp>

#define FileName "/values"

void LedStrip::preMeshSetup()
{

    MeshDevice::preMeshSetup();
    fileSystem.init();
    fileSystem.readStruct(FileName, runtimeValue);

    if (runtimeValue.version != 1)
    {
        runtimeValue.setMode("Off");
        runtimeValue.delay = 100;
        runtimeValue.numberOfLeds = 94;
        runtimeValue.brightness = 150;
        runtimeValue.step = 0,
        runtimeValue.reverse = false;
        runtimeValue.pixelColor.num = 0xFF000000;
        runtimeValue.version = 1;
    }

    // delay = wrapper.readIntIfExists("/delay", 100);
    // numberOfLeds = wrapper.readIntIfExists("/numLeds", 94);
    // brightness = wrapper.readIntIfExists("/bright", 150);
    // step = wrapper.readIntIfExists("/step", 0);
    // reverse = wrapper.readByteIfExists("/reverse", 0);
    // runtimeValue.pixelColor.num = wrapper.readIntIfExists("/pixelColor", 0);

    pinMode(4, OUTPUT);
    // pinMode(22, OUTPUT);
    pinMode(13, INPUT);
    pinMode(15, OUTPUT);
    pinMode(2, OUTPUT);
    pStrand = {.rmtChannel = 0, .gpioNum = 4, .ledType = LED_SK6812W_V1, .brightLimit = 255, .numPixels = runtimeValue.numberOfLeds};

    digitalWrite(15, HIGH);
    digitalWrite(2, LOW);

    if (digitalLeds_initStrands(&pStrand, 1))
    {
        while (true)
        {
        };
    }
    digitalLeds_resetPixels(&pStrand);

    OnMsgReceived(mesh.getNodeId(), "Update", runtimeValue.mode, std::vector<MessageParameter>());
}

void LedStrip::OnMeshMsgReceived(uint32_t from, const std::string &messageType, const std::string &command, const std::vector<MessageParameter> &parameter)
{
    // Serial.println("Recieved Command: " + command + " Type: " + messageType);
    if (messageType == "Update")
    {
        manualInputPin = true;

        if (command == "RGB")
        {
            runtimeValue.setMode(command);
            CreateTask<RGB>(runtimeValue.delay);
        }
        else if (command == "Strobo")
        {
            runtimeValue.setMode(command);
            CreateTask<Strobo>(runtimeValue.delay);
        }
        else if (command == "RGBCycle")
        {
            runtimeValue.setMode(command);
            CreateTask<RGBCycle>(runtimeValue.delay);
        }
        else if (command == "LightWander")
        {
            runtimeValue.setMode(command);
            CreateTask<LightWander>(runtimeValue.delay);
        }
        else if (command == "RGBWander")
        {
            runtimeValue.setMode(command);
            CreateTask<RGBWander>(runtimeValue.delay);
        }
        else if (command == "Reverse")
        {
        }
        else if (command == "SingleColor")
        {
            if (parameter.size() > 0)
            {
                int offset = 0;
                uint32_t color;
                SerializeHelper::deserialize(&color, parameter[0], offset);
                runtimeValue.pixelColor.num = color;
            }
            runtimeValue.setMode(command);
            CreateTask<SingleColor>(runtimeValue.delay);
        }
        else if (command == "Off")
        {
            runtimeValue.setMode(command);
            CreateTask<Off>(100);
            manualInputPin = false;
        }
        else if (command == "Mode")
        {
            runtimeValue.setMode(command);
            CreateTask<Mode>(0);
        }
    }
    else if (messageType == "Options")
    {
        std::string par;
        if (parameter.size() > 0)
            par = parameter[0].get();
        int offset = 0;
        if (command == "Delay")
        {
            int delay;
            SerializeHelper::deserialize(&delay, par, offset);
            runtimeValue.delay = delay;
            // Serial.println(par);
            Serial.println(runtimeValue.delay);
        }
        else if (command == "Brightness")
        {
            int brightness;

            SerializeHelper::deserialize(&brightness, par, offset);
            runtimeValue.brightness = brightness;
            // Serial.println(par);
            Serial.println(runtimeValue.brightness);
        }
        else if (command == "RelativeBrightness")
        {
        }
        else if (command == "Color")
        {
            uint32_t color;
            SerializeHelper::deserialize(&color, par, offset);
            runtimeValue.pixelColor.num = color;
            // Serial.println(par);
            Serial.println(runtimeValue.pixelColor.num);
        }
        else if (command == "Reverse")
        {
            runtimeValue.reverse = !runtimeValue.reverse;
        }
        else if (command == "Calibration")
        {
            int ledNum;
            SerializeHelper::deserialize(&ledNum, par, offset);
            runtimeValue.numberOfLeds = ledNum;
            pStrand = {.rmtChannel = 0, .gpioNum = 4, .ledType = LED_SK6812W_V1, .brightLimit = 255, .numPixels = runtimeValue.numberOfLeds};

            if (digitalLeds_initStrands(&pStrand, 1))
            {
                while (true)
                {
                };
            }
            digitalLeds_resetPixels(&pStrand);
        }
        LedStrip::OnMeshMsgReceived(from, "Update", runtimeValue.mode, std::vector<MessageParameter>());
    }

    if (from != mesh.getNodeId())
        saveCurrentState();
}

std::vector<MessageParameter> LedStrip::AdditionalWhoAmIResponseParams()
{
    return std::vector<MessageParameter>();
}

template <class T, class>
void LedStrip::CreateTask(int delay, bool sendToServer)
{
    if (ledLoopTask)
    {
        ledLoopTask->disable();
        userScheduler.deleteTask(*ledLoopTask);
        mode->Stop();
    }
    mode = Device::make_unique<T>();
    mode->Init(runtimeValue.pixelColor);
    // xTaskCreatePinnedToCore(ledLoopTask, "ledLoop", 1024, NULL, 0, &ledLoopTask, 0);
    ledLoopTask = Device::make_unique<Task>(TASK_MILLISECOND * delay, TASK_FOREVER, [this]()
                                            {
                                                this->mode->Run(pStrand, runtimeValue.pixelColor, runtimeValue.brightness, runtimeValue.numberOfLeds, runtimeValue.reverse, runtimeValue.step);
                                                digitalLeds_updatePixels(&pStrand);
                                                if (mode->steppingEnabled)
                                                {
                                                    runtimeValue.step += this->runtimeValue.reverse ? -1 : 1;
                                                    runtimeValue.step = (runtimeValue.step + runtimeValue.numberOfLeds) % runtimeValue.numberOfLeds;
                                                }
                                            });
    userScheduler.addTask(*ledLoopTask);
    /*
    char mode[32];
    int delay;
    int numberOfLeds;
    int brightness;
    uint32_t step;
    bool reverse;
    pixelColor_t pixelColor;
    uint16_t version;
    */
    uint8_t index = 0;
    for (auto c : runtimeValue.mode)
    {
        if (c == '\0')
            break;
        ++index;
    }
    if (sendToServer)
    {

        std::string stdStr(&runtimeValue.mode[0], index);

        Serial.printf("Sending mode %s\n", stdStr.c_str());
        MeshDevice::sendSingle(1, "Update", "Mode", {stdStr, runtimeValue.delay, runtimeValue.numberOfLeds, runtimeValue.brightness, runtimeValue.step, runtimeValue.reverse, runtimeValue.pixelColor.num, runtimeValue.version});
    }
    ledLoopTask->enable();
}

void LedStrip::loop()
{
    mesh.update();
    auto newState = digitalRead(13);

    if (newState == HIGH && newState != oldPinState)
    {
        oldPinState = newState;
        Serial.println("Switch to manual mode on");
        if (runtimeValue.pixelColor.num == 0)
            runtimeValue.pixelColor.num = 0xFF000000;
        runtimeValue.setMode("SingleColor");
        CreateTask<SingleColor>(runtimeValue.delay, false);
    }
    else if (newState == LOW && newState != oldPinState)
    {
        oldPinState = newState;
        runtimeValue.setMode("Off");
        CreateTask<Off>(runtimeValue.delay, false);
        Serial.println("Switch to manual mode off");
    }
}

void LedStrip::preReboot()
{
    saveCurrentState();
}

void LedStrip::restartMesh()
{
    Serial.println("Restarting Mesh LedStrip");

    saveCurrentState();

    MeshDevice::restartMesh();
}

void LedStrip::saveCurrentState()
{
    LedStripValue value;
    fileSystem.readStruct(FileName, value);
    if (value != runtimeValue)
    {
        fileSystem.writeStruct(FileName, runtimeValue);
    }
}
