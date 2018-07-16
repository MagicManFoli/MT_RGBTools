#ifndef MT_RGBTOOLS_H_INCLUDED
#define MT_RGBTOOLS_H_INCLUDED

/*--------------------------------------------------*\

                (c) Robin Modisch
                Modisch Fabrications

\*--------------------------------------------------*/

/*----------------------------------------*\

 Fragen:
  - Performance-Unterschied inline-Funktion und einsetzen
  - inline nur deklaration oder auch definition?

  Threads(bei mehreren Kernen)


\*----------------------------------------*/


#include <stdint-gcc.h>
//#include "../Ardu_sim.h"  //comment in
#include <Arduino.h>        //comment out

#define MAX_U_16 60000      //1min
#define MAX_U_32 8.64e+7   //1 Day


enum Task_Code : uint8_t
{
    IDLE = 0,       //needed?
    FADE = 1,
    BLINK = 2,
    BURST = 4,
    //... = 8,
    //... = 16,
    //... = 32,
    //... = 64,
    UNDEF = 128
};

enum OUT_Code : uint8_t
{
    NO_ERROR = 0,               // ?
    ERR_INPUT = 1,              // red
    ERR_INPUT_RANGE = 2,        // 2x red
    //... = 4,                  // green
    //... = 8,                  // 2x green
    //... = 16,                 // blue
    //... = 32,                 // 2x blue
    //... = 64,                 // white
    ERR_UNDEF = 128             // 2x white
};


class vect
{
public:
    //vect () : r(0), g(0), b(0) {}
    vect (const uint8_t r, const uint8_t g, const uint8_t b) : r (r), g (g), b (b) {}

    uint8_t r, g, b;
};

class RGB : public vect
{
public:
    RGB () : vect (0, 0, 0) {}
    RGB (const uint8_t red, const uint8_t green, const uint8_t blue) : vect (red, green, blue) {}
    bool operator== (const RGB b) const ;
};

class signed_RGB
{
public:
    int16_t r, g, b;
};

class MT_RGB
{
public:
    MT_RGB (const uint8_t r, const uint8_t g, const uint8_t b);   //ctor, set Output Pins
    virtual ~MT_RGB();

    inline bool has_Task (const Task_Code) const;            //check for Task [code] to exist
    inline uint8_t has_Task () const;                     // check for existing task

    inline bool set_Task (const Task_Code);
    inline bool clear_Task (const Task_Code);

    inline void save_Tasks ();
    inline void restore_Tasks ();

    inline void save_Current ();
    inline void restore_Current ();

    unsigned long get_time() const;
    void update();                              //update for fade and other timing-critical events

    void fade_next_step();
    void burst_next_step();

    void set (const RGB dest);                             //directly set Colors
    void start_fade (const RGB dest, const uint32_t duration);     //calculates parameters for fade-steps, sets Task
    void start_burst (const RGB dest, const uint8_t n, const uint16_t delay);          //stop every other Task, turn off RGB, set params
    //void set_rand_blink(const uint16_t duration);
    //void set_rand_fade(const uint32_t duration);

    void set_OUT (const OUT_Code);                   //calculates parameters for blinking, sets Task

private:
    const vect pins;                                  //Hardware Pins

    RGB curr;       //current color
    RGB curr_backup;

    uint8_t task;
    uint8_t task_backup = 0;

    unsigned long start_time;           //time since beginning, set on construction

    ///fade-params
    RGB fade_dest;
    uint32_t fade_delay;
    unsigned long t_last_fade;            //time since last step

    ///burst-params
    RGB c_burst;
    uint8_t n_burst;                    //times to change
    uint16_t burst_delay;
    unsigned long t_last_burst;

};




#endif // MT_RGBTOOLS_H_INCLUDED
