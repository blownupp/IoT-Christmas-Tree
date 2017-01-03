// This #include statement was automatically added by the Particle IDE.
#include "WebServer/WebServer.h"

#define PREFIX "/neo"
WebServer webserver(PREFIX, 80);
//#include "helper_functions.h"

// This #include statement was automatically added by the Particle IDE.
#include "neopixel/neopixel.h"
#define PIN A3
#define NUM_LEDS 240

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, WS2812B);

class Fader
{
    // Specific settings for each LED instance
    int LEDPixel;                   // chosen at random
    long sparkleDelay;              // before fading
    long fadeSpeed;                 // how fast/slow to fade
    
    // Maintain LED state
    bool LEDState;
    unsigned long fPreviousMillis;  // for timing on time and speed of fade
    unsigned long sPreviousMillis;  // ... timing how long LED is LIT
    
    // Fade var?
    uint8_t StartColor;
    uint8_t CurrentColor;
    uint8_t nextColor;
    
    
    // Constructor - creates a Flasher 
    // and initializes the member variables and state
    public:
    Fader(int pixel, long delay, long speed)
    {
        LEDPixel = pixel;
        
        //sPreviousMillis
        sparkleDelay = delay;
        
        // fPreviousMillis
        fadeSpeed = speed;
        
        // LEDState starts ON
        LEDState = 1;
        
        // Two different timers running
        fPreviousMillis, sPreviousMillis = 0;
        
        // Start off as WHITE
        StartColor = 0xff;
        CurrentColor = 0xff;
        
        // Ends up slowely fading to black
        nextColor = ( ( CurrentColor * .5 ) + ( CurrentColor * .25 ) );
        
        strip.setPixelColor(LEDPixel, StartColor, StartColor, StartColor);
        
    }
    
    void NewPixel(int pixel)
    {
        
        if (LEDState == 0)
        {
            
            LEDPixel = pixel;
            
            strip.setPixelColor(LEDPixel, StartColor, StartColor, StartColor);
            LEDState = 1;
            
        }
        
    }
    
    bool IsLit()
    {
        
        return LEDState;
        
    }
    
    void Update()
    {
        
        // Check to see if it's time to change the state of the LED
        // and start timer for SPARKLE delay
        unsigned long sCurrentMillis = millis();
        unsigned long fCurrentMillis = millis();
        
        // Get current color of LED
        CurrentColor = Get8( strip.getPixelColor( LEDPixel ) );
        
        if ( LEDState && ( sCurrentMillis - sPreviousMillis >= sparkleDelay ) )
        {
            
            sPreviousMillis, sCurrentMillis = 0;
            
            // if the LED is lit and has been on longer than SparkleDelay
            if ( ( CurrentColor <= 15 ) && ( fCurrentMillis - fPreviousMillis >= fadeSpeed ) )
            {
                nextColor = CurrentColor - 5;
                CurrentColor = nextColor;
                
                if ( CurrentColor < 7 )
                {
                    fPreviousMillis = fCurrentMillis;
                    strip.setPixelColor(LEDPixel, 0, 0, 0);
                    LEDState = 0;
                    
                    // To avoid reinitializing a class instance every time an LED runs through
                    // the animation, once its set to OFF, we'll reset the LED
                }
                else
                {
                    fPreviousMillis = fCurrentMillis;
                    strip.setPixelColor(LEDPixel, CurrentColor, CurrentColor, CurrentColor);
                }
                
            }
            else
            {
                
                nextColor = ((CurrentColor * .5) + (CurrentColor * .25));
                CurrentColor = nextColor;
                
                strip.setPixelColor(LEDPixel, CurrentColor, CurrentColor, CurrentColor);
                
                // Reset timer for FADE speed
                fPreviousMillis = fCurrentMillis;
                
            }
            
            //sPreviousMillis = sCurrentMillis;
            
            
        }
        
        //return LEDState;
    }
};

void SetupFader()
{
    char i;
    // Loop through to setup 24 instances of the LED Fader class with a random LED,
    // a random duration before fading and a constant fade speed of 50ms per brightness level

    for ( i = 1; i < 25; i++ )
    {
        Fader i( random(0 , NUM_LEDS - 1 ), random(1 , 100), 50 );
    }
}

char buff[64];
int len = 64;

int neoPattern = 1;
/* commands are functions that get called by the webserver framework
 * they can read any posted data from client, and they output to the
 * server to send data back to the web browser. */
void neoCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (type == WebServer::POST)
  {
    bool repeat;
    char name[16], value[16];
    do
    {
      /* readPOSTparam returns false when there are no more parameters
       * to read from the input.  We pass in buffers for it to store
       * the name and value strings along with the length of those
       * buffers. */
      repeat = server.readPOSTparam(name, 16, value, 16);

      /* this is a standard string comparison function.  It returns 0
       * when there's an exact match.  We're looking for a parameter
       * named "buzz" here. */
      if (strcmp(name, "neo") == 0)
      {
	/* use the STRing TO Unsigned Long function to turn the string
	 * version of the delay number into our integer buzzDelay
	 * variable */
        neoPattern = strtoul(value, NULL, 10);
      }
    } 
    while (repeat);
    
    // after procesing the POST data, tell the web browser to reload
    // the page using a GET method. 
    server.httpSeeOther(PREFIX);
    return;
  }

  /* for a GET or HEAD, send the standard "it's all OK headers" */
  server.httpSuccess();

  /* we don't output the body for a HEAD request */
  if (type == WebServer::GET)
  {
    /* store the HTML in program memory using the P macro */
    P(message) = 
      "<html><head><title>Christmas Tree Patterns</title>"
      "<body>"
      "<h1>Make your selection!</h1>"
      "<form action='/neo' method='POST'>"
      "<p><button name='neo' value='0'>Turn if Off!</button></p>"
      "<p><button name='neo' value='1'>Red/Green/Gold Sparkle</button></p>"
      "<p><button name='neo' value='2'>White Snow</button></p>"
      "<p><button name='neo' value='3'>Rainbow Sparkle</button></p>"
      "<p><button name='neo' value='4'>Rainbow Cycle</button></p>"
      "</form></body></html>";

    server.printP(message);
  }
}



void setup() {
  /* setup our default command that will be run when the user accesses
   * the root page on the server */
  webserver.setDefaultCommand(&neoCmd);
  
  //webserver.addCommand("index.html", &neoCmd);

  /* run the same command if you try to load /index.html, a common
   * default page name */
  //webserver.addCommand("index.html", &neoCmd);

  /* start the webserver */
  webserver.begin();
  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  SetupFader(); // Initialize Fader class with 24 instances
}



// *** REPLACE FROM HERE ***
void loop() { 
    //runSnowSparkle(random(15,90));
    webserver.processConnection(buff, &len);
    
    switch (neoPattern)
    {
        case 0:
            for (int i = 0; i < NUM_LEDS; i++)
            {
                strip.setPixelColor(i,0,0,0);
            }
            strip.show();
            break;
        
        case 1:
            runSparkle(1);
            break;
        
        case 2:
            runSnowSparkle(1);
            break;
        
        case 3:
            runrainbowSparkle(1);
            break;
        
        case 4:
            rainbowCycle(20);
            break;
    }
    //runSparkle(random(15,90));
    //runrainbowSparkle(random(15,90));
}


//***** RUN ROUTINES ***********************

void runSparkle(int duration) {
    // duration in seconds * 1000 for millis
    unsigned long durationMillis = duration * 1000;
    unsigned long theTimenow = millis();
    
    while((millis() - theTimenow) < durationMillis){
        Sparkle(random(255),0,0,random(0,50));
        Sparkle(0,random(255),0,random(0,50));
        Sparkle(random(180,255),random(100,200),0,random(0,50));
    }
}

void runrainbowSparkle(int duration) {
    unsigned long durationMillis = duration * 1000;
    unsigned long theTimenow = millis();
    
    while((millis() - theTimenow) < durationMillis){
        rainbowSparkle(random(0,50));
    }
}

void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}


void runSnowSparkle(int duration) {
    unsigned long durationMillis2 = duration * 1000;
    unsigned long theTimenow2 = millis();
    /*
    while((millis() - theTimenow2) < durationMillis2) {
        SnowSparkle(0,0,0,random(10,250),random(25,300));
    }
    */
    while((millis() - theTimenow2) < durationMillis2) {
        SnowSparkle(random(10,250),random(25,300));
    }
}

//****************************************************


void Sparkle(byte red, byte green, byte blue, int SpeedDelay) {
  int Pixel = random(NUM_LEDS);
  setPixel(Pixel,red,green,blue);
  showStrip();
  delay(SpeedDelay);
  setPixel(Pixel,0,0,0);
}

void rainbowSparkle(int SpeedDelay) {
  for(int j=0; j<256; j++) { // 1 cycle of all colors on wheel
      int i = random(NUM_LEDS);
      strip.setPixelColor(i, Wheel(((i * 256 / NUM_LEDS) + j) & 255));
      strip.show();
      delay(SpeedDelay);
      strip.setPixelColor(i, 0,0,0);
  }

}
/*
void SnowSparkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay) {
  setAll(red,green,blue);
  
  int Pixel = random(NUM_LEDS);
  setPixel(Pixel,0xff,0xff,0xff);
  showStrip();
  delay(SparkleDelay);
  setPixel(Pixel,red,green,blue);
  showStrip();
  delay(SpeedDelay);
}
*/

// Lets save the randomly chosen LED numbers into an array to protect against chosing the same ones
// over again



//yte ledArray[24];
/*
void SetupFader ( )
{
    int i;
    // Loop through to setup 24 instances of the LED Fader class with a random LED,
    // a random duration before fading and a constant fade speed of 50ms per brightness level
    
    for ( i = 1; i < 25; i++ )
    {
        
        //ledArray[i - 1] = random(0 , NUM_LEDS - 1 );
        
        Fader i( random(0 , NUM_LEDS - 1 ), random(1 , 100), 50 );
        
    }
    
    
    
}
*/
void ProcessFader ( )
{
    strip.show();
    
    for (char i = 1; i < 25; i++ )
    {
        
        i.Update();
        strip.show();
        
        if ( !( i.IsLit() ) )
        {
            
            i.NewPixel( random( 0 , NUM_LEDS - 1 ) );
            strip.show();
            
        }
        
    }    
    
}



uint8_t litLeds[2][24];
int snowSparkleCounter = 0;

void SnowSparkle(int SparkleDelay, int SpeedDelay) {
  int Pixel = random(NUM_LEDS);
  litLeds[0][snowSparkleCounter] = Pixel;
  
  setPixel(Pixel,0xff,0xff,0xff);
  showStrip();
  
  litLeds[1][snowSparkleCounter] = Get8(strip.getPixelColor(Pixel));
  
    // Iterate through array and dim remaining LEDs
  for(int h=0; h<24; h++){
      if(litLeds[1][h] != 0 && litLeds[0][h] != 0){
          litLeds[1][h] = litLeds[1][h] >> 1;
      }
  }
  
  delay(SparkleDelay);
  
  // Iterate through the array and light up only stored LEDs
  for(int i=0; i<NUM_LEDS; i++){
      for(int n=0; n<24; n++){
            if(i != litLeds[0][n]){
                strip.setPixelColor(i, 0, 0, 0);
            }
            else{
                byte wLevel = litLeds[1][n];
                strip.setPixelColor(litLeds[0][n], wLevel, wLevel, wLevel);
          }
      }

  }
  

  strip.show();
  
  delay(SpeedDelay);
  
  snowSparkleCounter += 1;
  if(snowSparkleCounter > 23){
      snowSparkleCounter = 0;
  }
  
}

//*****************************************************

void showStrip() {
   strip.show();
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue); 
  }
  showStrip();
}


void ColorSet(uint32_t color)
{
    
    for (int i = 0; i < NUM_LEDS; i++)
    {
    strip.setPixelColor(i, color);
    }
    
    strip.show();
}

// Return 8bit portion of 32bit white pixel
uint8_t Get8(uint32_t color)
{
    return (color >> 16) & 0xFF;
}

// Extract Red from uint32_t
uint8_t Red(uint32_t color)
{
    return (color >> 16) & 0xFF;
}

// Get Green from uint32_t
uint8_t Green(uint32_t color)
{
    return (color >> 8) & 0xFF;
}

// ... and Blue
uint8_t Blue(uint32_t color)
{
    return (color & 0xFF);
}


uint32_t Wheel(byte WheelPos) 
{
  if(WheelPos < 85) 
  {
      return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) 
  {
      WheelPos -= 85;
      return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else 
  {
      WheelPos -= 170;
      return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
