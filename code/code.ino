#include <LiquidCrystal.h>

/*
 * The note used in the buzzer.
 */
constexpr uint32_t kNOTE_C5 = 526;

/*
 * Drivers for the components of
 * the project.
 */
namespace driver {

/*
 * Pins used in the project.
 */
namespace pin {

constexpr uint32_t kRelay = 4;
constexpr uint32_t kBuzzer = 3;
constexpr uint32_t kSwitch = 2;
constexpr uint32_t kRS = 13;
constexpr uint32_t kEN = 12;
constexpr uint32_t kD4 = 11;
constexpr uint32_t kD5 = 10;
constexpr uint32_t kD6 = 9;
constexpr uint32_t kD7 = 8;

}

/*
 * Control layer for the 16x2 LCD.
 */
class Display {
  public:
    Display() : display_(LiquidCrystal(
              pin::kRS,
              pin::kEN,
              pin::kD4,
              pin::kD5,
              pin::kD6,
              pin::kD7
            )) {
      display_.begin(16, 2);
      MainScreen();
    }

    virtual ~Display() = default;

    void StartupScreen() {
      PrintTitle();

      display_.setCursor(1, 1);
      display_.print("Ligando em 3s");
      delay(1000);

      display_.setCursor(12, 1);
      display_.print('2');
      delay(1000);

      display_.setCursor(12, 1);
      display_.print('1');
      delay(1000);
    }

    void MainScreen(bool state = false) {
      PrintTitle();

      char const* state_str = (state)? 
        "Ligada" : "Desligada";

      if(state) {
        display_.setCursor(5, 1);
      } else {
        display_.setCursor(3, 1);
      }

      display_.print(state_str);
    }

  private:
    void PrintTitle() {
      display_.clear();
      display_.setCursor(0, 0);
      display_.print("Bobina de Tesla");
    }
    
  private:
    LiquidCrystal display_;
}; 

/*
 * Control layer for the buzzer.
 */
class Buzzer {
  public:
    Buzzer(uint32_t note_frequency) : 
          note_(note_frequency), 
          buzzer_pin_(pin::kBuzzer) {
      pinMode(buzzer_pin_, OUTPUT);
    }

    virtual ~Buzzer() = default;

    void PlayNote() const {
      tone(buzzer_pin_, kNOTE_C5);
      delay(500);
      noTone(buzzer_pin_);
    }
  private:
    uint32_t note_;
    uint32_t buzzer_pin_;
};

/*
 * Provides control over the
 * relay that control the power
 * supply of the coil.
 */
class SupplyControl {
  public:
    SupplyControl() : 
        switch_pin_(pin::kSwitch), 
        relay_pin_(pin::kRelay), 
        is_on_(false) {
      pinMode(relay_pin_, OUTPUT);
      pinMode(switch_pin_, INPUT_PULLUP);
    }

    void TogglePower() {
      is_on_ = !is_on_;
      digitalWrite(relay_pin_, is_on_);
    }

    bool IsOn() const {
      return is_on_;
    }

    uint32_t GetSwitchPin() const {
      return switch_pin_;
    }

    uint32_t GetRelayPin() const {
      return relay_pin_;
    }

  private:
    uint32_t switch_pin_;
    uint32_t relay_pin_;
    bool is_on_;
};

}

driver::Display drv_display;
driver::Buzzer drv_buzzer(kNOTE_C5);
driver::SupplyControl drv_supply_control;

/*
 * Handles the event of turning the
 * coil on and off.
 */
void ISR_ActivateCoil() {
  if (!drv_supply_control.IsOn()) {
    drv_buzzer.PlayNote();
    drv_display.StartupScreen();
    drv_buzzer.PlayNote();
  }
  drv_supply_control.TogglePower();
  drv_display.MainScreen(drv_supply_control.IsOn());
}

void setup() {
  attachInterrupt(digitalPinToInterrupt(drv_supply_control.GetSwitchPin()), ISR_ActivateCoil, CHANGE);
}

void loop() {}
