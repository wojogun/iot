#define led_y 12  // Define yellow LED pin as 12

void setup()  
{    // The code inside setup function runs only once  
  pinMode(led_y, OUTPUT);  // Set the pin as output mode  
}

void loop()  
{     // The code inside loop function runs repeatedly  
  // Blink 3 times
  for(int i = 0; i < 3; i++) {
    digitalWrite(led_y, HIGH);  // Set led_y pin to HIGH (turns on the LED)  
    delay(200);     // Delay in milliseconds  
    digitalWrite(led_y, LOW);   // Set led_y pin to LOW (turns off the LED)  
    delay(200);  
  }
  
  // Wait 2 seconds before next 3-blink sequence
  delay(2000);
}