#include <LedStrip.hpp>
#include <Modes/Modes.hpp>

void LedStrip::init()
{
    delay = 100;
    numberOfLeds = 94;
    brightness = 150;
    step = 0;
    pinMode(4, OUTPUT);
    pinMode(16, INPUT);
    pStrand = {.rmtChannel = 0, .gpioNum = 4, .ledType = LED_SK6812W_V1, .brightLimit = 255, .numPixels = numberOfLeds};

    if (digitalLeds_initStrands(&pStrand, 1))
    {
        while (true)
        {
        };
    }
    digitalLeds_resetPixels(&pStrand);
}

void LedStrip::OnMsgReceived(uint32_t from, const String &messageType, const String &command, const String &parameter)
{
    Serial.println("Recieved Command: " + command + " Type: " + messageType);
    if (messageType == "Update")
    {
        lastUpdateCommand = command;
        manualInputPin=true;
        if (command == "RGB")
        {
            CreateTask<RGB>(delay);
        }
        else if (command == "Strobo")
        {
            CreateTask<Strobo>(delay);
        }
        else if (command == "RGBCycle")
        {
            CreateTask<RGBCycle>(delay);
        }
        else if (command == "LightWander")
        {
            CreateTask<LightWander>(delay);
        }
        else if (command == "RGBWander")
        {
            CreateTask<RGBWander>(delay);
        }
        else if (command == "Reverse")
        {
        }
        else if (command == "SingleColor")
        {
            if (parameter.length() > 0)
            {
                auto par = parameter.substring(2, parameter.length() - 2);
                pixelColor.num = std::strtoul(par.c_str(), 0, 0);
            }
            CreateTask<SingleColor>(delay);
        }
        else if (command == "Off")
        {
            CreateTask<Off>(100);
            manualInputPin=false;
        }
        else if (command == "Mode")
        {
            CreateTask<Mode>(0);
        }
    }
    else if (messageType == "Options")
    {
        auto par = parameter.substring(2, parameter.length() - 2);
        if (command == "Delay")
        {
            delay = std::strtoul(par.c_str(), 0, 0);
            Serial.println(par);
            Serial.println(delay);
        }
        else if (command == "Brightness")
        {
            brightness = std::strtoul(par.c_str(), 0, 0);
            Serial.println(par);
            Serial.println(brightness);
        }
        else if (command == "RelativeBrightness")
        {
        }
        else if (command == "Color")
        {
            pixelColor.num = std::strtoul(par.c_str(), 0, 0);
            Serial.println(par);
            Serial.println(pixelColor.num);
        }
        else if (command == "Reverse")
        {
            reverse = !reverse;
        }
        else if (command == "Calibration")
        {
            numberOfLeds = std::strtoul(par.c_str(), 0, 0);
            pStrand = {.rmtChannel = 0, .gpioNum = 4, .ledType = LED_SK6812W_V1, .brightLimit = 255, .numPixels = numberOfLeds};

            if (digitalLeds_initStrands(&pStrand, 1))
            {
                while (true)
                {
                };
            }
            digitalLeds_resetPixels(&pStrand);
        }
        LedStrip::OnMsgReceived(from, "Update", lastUpdateCommand, "");
    }
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
    mode->Init(pixelColor);
    ledLoopTask = Device::make_unique<Task>(TASK_MILLISECOND * delay, TASK_FOREVER, [this]() {
        this->mode->Run(pStrand, pixelColor, brightness, numberOfLeds, reverse, step);
        digitalLeds_updatePixels(&pStrand);
        step += this->reverse ? -1 : 1;
        step = (step + numberOfLeds) % numberOfLeds;
    });
    userScheduler.addTask(*ledLoopTask);
    auto name = typeid(T).name();
    Device::sendSingle(1, "Update", "Mode", {name, brightness, pixelColor.num, numberOfLeds});
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
                pixelColor.num = 0xFF000000;
                CreateTask<SingleColor>(delay);
            }
            else
            {
                CreateTask<Off>(delay);
                Serial.println("Switch to manual mode off");
            }
        }
    }
    else if (newState == LOW && newState != oldPinState)
    {
        oldPinState = newState;
    }
}