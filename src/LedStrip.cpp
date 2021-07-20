#include <LedStrip.hpp>
#include <Modes/Modes.hpp>

void LedStrip::preMeshSetup()
{
    MeshDevice::preMeshSetup();
    Serial.println(micros());
    fileSystem.init();

    Serial.println(micros());
    fileSystem.readStruct("/values", runtimeValue);

    if (runtimeValue.version != 1)
    {
        strcpy(runtimeValue.mode, "Off");
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

    Serial.println(micros());
    pinMode(4, OUTPUT);
    pinMode(22, OUTPUT);
    pinMode(19, OUTPUT);
    pinMode(16, INPUT);
    pStrand = {.rmtChannel = 0, .gpioNum = 4, .ledType = LED_SK6812W_V1, .brightLimit = 255, .numPixels = runtimeValue.numberOfLeds};

    digitalWrite(22, LOW);
    digitalWrite(19, HIGH);

    if (digitalLeds_initStrands(&pStrand, 1))
    {
        while (true)
        {
        };
    }
    digitalLeds_resetPixels(&pStrand);

    Serial.println(micros());

    OnMsgReceived(mesh.getNodeId(), "Update", runtimeValue.mode, std::vector<MessageParameter>());
    Serial.println(micros());
}

void LedStrip::OnMeshMsgReceived(uint32_t from, const std::string &messageType, const std::string &command, const std::vector<MessageParameter> &parameter)
{
    // Serial.println("Recieved Command: " + command + " Type: " + messageType);
    if (messageType == "Update")
    {
        manualInputPin = true;
        if (command == "RGB")
        {
            CreateTask<RGB>(runtimeValue.delay);
        }
        else if (command == "Strobo")
        {
            CreateTask<Strobo>(runtimeValue.delay);
        }
        else if (command == "RGBCycle")
        {
            CreateTask<RGBCycle>(runtimeValue.delay);
        }
        else if (command == "LightWander")
        {
            CreateTask<LightWander>(runtimeValue.delay);
        }
        else if (command == "RGBWander")
        {
            CreateTask<RGBWander>(runtimeValue.delay);
        }
        else if (command == "Reverse")
        {
        }
        else if (command == "SingleColor")
        {
            if (parameter.size() > 0)
            {
                auto par = parameter[0];
                runtimeValue.pixelColor.num = std::strtoul(par.get().c_str(), 0, 0);
            }
            CreateTask<SingleColor>(runtimeValue.delay);
        }
        else if (command == "Off")
        {
            CreateTask<Off>(100);
            manualInputPin = false;
        }
        else if (command == "Mode")
        {
            CreateTask<Mode>(0);
        }
    }
    else if (messageType == "Options")
    {
        auto par = parameter[0].get();
        if (command == "Delay")
        {
            runtimeValue.delay = std::strtoul(par.c_str(), 0, 0);
            // Serial.println(par);
            Serial.println(runtimeValue.delay);
        }
        else if (command == "Brightness")
        {
            runtimeValue.brightness = std::strtoul(par.c_str(), 0, 0);
            // Serial.println(par);
            Serial.println(runtimeValue.brightness);
        }
        else if (command == "RelativeBrightness")
        {
        }
        else if (command == "Color")
        {
            runtimeValue.pixelColor.num = std::strtoul(par.c_str(), 0, 0);
            // Serial.println(par);
            Serial.println(runtimeValue.pixelColor.num);
        }
        else if (command == "Reverse")
        {
            runtimeValue.reverse = !runtimeValue.reverse;
        }
        else if (command == "Calibration")
        {
            runtimeValue.numberOfLeds = std::strtoul(par.c_str(), 0, 0);
            pStrand = {.rmtChannel = 0, .gpioNum = 4, .ledType = LED_SK6812W_V1, .brightLimit = 255, .numPixels = runtimeValue.numberOfLeds};

            if (digitalLeds_initStrands(&pStrand, 1))
            {
                while (true)
                {
                };
            }
            digitalLeds_resetPixels(&pStrand);
        }
        LedStrip::OnMsgReceived(from, "Update", runtimeValue.mode, std::vector<MessageParameter>());
    }

    if (from != mesh.getNodeId())
        saveCurrentState();
}

std::vector<MessageParameter> LedStrip::AdditionalWhoAmIResponseParams()
{
    return std::vector<MessageParameter>();
}

template <class T, class>
void LedStrip::CreateTask(int delay)
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
    ledLoopTask = Device::make_unique<Task>(TASK_MILLISECOND * delay, TASK_FOREVER, [this]() {
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
    MeshDevice::sendSingle(1, "Update", "Mode", {runtimeValue.mode, runtimeValue.delay, runtimeValue.numberOfLeds, runtimeValue.brightness, runtimeValue.step, runtimeValue.reverse, runtimeValue.pixelColor.num, runtimeValue.version});
    ledLoopTask->enable();
}

void LedStrip::loop()
{
    mesh.update();
    auto newState = digitalRead(16);

    if (newState == HIGH && newState != oldPinState)
    {
        timeval tv;
        gettimeofday(&tv, NULL);
        if (lastTime + 1 < tv.tv_sec)
        {
            lastTime = tv.tv_sec;
            oldPinState = newState;
            manualInputPin = !manualInputPin;
            if (manualInputPin)
            {
                Serial.println("Switch to manual mode on");
                if (runtimeValue.pixelColor.num == 0)
                    runtimeValue.pixelColor.num = 0xFF000000;
                CreateTask<SingleColor>(runtimeValue.delay);
                strcpy(runtimeValue.mode, "SingleColor");
            }
            else
            {
                CreateTask<Off>(runtimeValue.delay);
                Serial.println("Switch to manual mode off");
                strcpy(runtimeValue.mode, "Off");
            }
        }
    }
    else if (newState == LOW && newState != oldPinState)
    {
        oldPinState = newState;
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
    fileSystem.readStruct("/values", value);
    if (value != runtimeValue)
    {
        fileSystem.writeStruct("/values", runtimeValue);
    }
}
