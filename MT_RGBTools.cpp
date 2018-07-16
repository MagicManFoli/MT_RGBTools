#include "MT_RGBTools.h"



/**--------------- Tools -------------------------------*/
template <typename T>
inline T max_T (const T a, const T b, const T c)
{
    int max = ( a < b ) ? b : a;
    return ( ( max < c ) ? c : max );
}

uint8_t IncDec (uint8_t& curr, const uint8_t& dest)
{
    if (curr < dest) curr++;
    else if (curr > dest) curr--;
    else return 0;
    return 1;
}

/**-----------------------------------------------------*/

MT_RGB::MT_RGB (const uint8_t r, const uint8_t g, const uint8_t b)
    : pins (r, g, b)
    , curr (0, 0, 0)
{
    this->task = IDLE;
    this->start_time = millis();
}

MT_RGB::~MT_RGB()
{
    //useless, but can't deactivate
}


bool RGB::operator== (const RGB b) const
{
    if (this->r == b.r && this->g == b.g && this->b == b.b) return 1;
    return 0;
}

bool MT_RGB::has_Task (const Task_Code code) const
{
    return ( (task & code) == 0) ? 0 : 1;
}

uint8_t MT_RGB::has_Task () const
{
    return (task == 0) ? 0 : task;
}

bool MT_RGB::set_Task (const Task_Code code)       //seems to work
{
    if (!has_Task (code) )
    {
        task |= code;
        return 1;
    }
    return 0;
}

bool MT_RGB::clear_Task (const Task_Code code)
{
    if (has_Task (code) )
    {
        task &= ~code;
        return 1;
    }
    return 0;
}

void MT_RGB::save_Tasks()
{
    task_backup = task;
}

void MT_RGB::restore_Tasks()
{
    task = task_backup;
}

void MT_RGB::save_Current()
{
    curr_backup = curr;
}

void MT_RGB::restore_Current()
{
    set (curr_backup);
}

unsigned long MT_RGB::get_time() const
{
    return (millis() - start_time);
}

void MT_RGB::update()
{
    if (has_Task() == 0 ) return;      //jump out of function

    //Serial.println ("Has Task, updating...\nTask is :");
    //Serial.print (task);

    unsigned long curr_time = get_time();  //time always relative to starting time

    if (has_Task (FADE) && (curr_time - t_last_fade) > fade_delay)   //time since beginning - time since last call > waiting time
    {
        fade_next_step();
    }

    if (has_Task (BURST) && (curr_time - t_last_burst) > burst_delay)
    {
        burst_next_step();
    }
}

void MT_RGB::fade_next_step()    //next step if enough time has passed
{
    if (IncDec (curr.r, fade_dest.r) ) analogWrite (pins.r, curr.r);
    if (IncDec (curr.g, fade_dest.g) ) analogWrite (pins.g, curr.g);
    if (IncDec (curr.b, fade_dest.b) ) analogWrite (pins.b, curr.b);

    if (curr == fade_dest)
    {
        clear_Task (FADE);
    }
    this->t_last_fade = get_time();
}

void MT_RGB::burst_next_step()
{
    if (n_burst > 0)                          //not finished yet
    {
        if (curr == RGB (0, 0, 0) ) {set (c_burst); }
        else if (curr == c_burst) {set (RGB (0, 0, 0) ); n_burst--;}
        this->t_last_burst = get_time();

    }
    else            //finished
    {
        clear_Task (BURST);
        restore_Tasks();
        restore_Current();
    }
}

void MT_RGB::set (const RGB dest)
{
    curr = dest;

    analogWrite (pins.r, curr.r);
    analogWrite (pins.g, curr.g);
    analogWrite (pins.b, curr.b);
}

void MT_RGB::start_fade (const RGB dest, const uint32_t duration) ///duration in s
{
    if (duration >  MAX_U_32)
    {
        set_OUT (ERR_INPUT_RANGE);
        return;
    }

    set_Task (FADE);

    signed_RGB diff;

    diff.r = dest.r - curr.r;
    diff.g = dest.g - curr.g;
    diff.b = dest.b - curr.b;

    uint8_t steps = max_T (abs (diff.r), abs (diff.g), abs (diff.b) );

    /*--- set Fade params ---*/
    this->fade_delay = duration / steps;
    this->fade_dest = dest;
    this->t_last_fade = get_time();

    Serial.println ("Fade set");    ///ardu
}

void MT_RGB::start_burst (const RGB dest, const uint8_t n, const uint16_t delay)
{
    if (delay >  MAX_U_16)
    {
        set_OUT (ERR_INPUT_RANGE);      //endless loop?
        return;
    }

    this->c_burst = dest;
    this->n_burst = n;
    this->burst_delay = delay / n;

    save_Current();
    save_Tasks();

    /*--- clear Tasks not meant to run while OUT ---*/
    clear_Task(FADE);
    /*----------------------------------------------*/

    set_Task (BURST);
    set (RGB (0, 0, 0) );
    t_last_burst = get_time();
    Serial.println ("Burst set");    ///ardu
}


void MT_RGB::set_OUT (const OUT_Code code)
{
    // get the appropriate color and timing of blinking

}




