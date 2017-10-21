#include <ESP8266WiFi.h>

// Thong so WiFi 
const char* ssid = "Phong Ky Thuat";
const char* password = "123456789";

// Tao server
WiFiServer server(80);

// Pin
int up_pin = 5;
int down_pin = 4;
int left_pin = 14;
int right_pin = 12;

void setup() {

  // Khoi dong Serial
  Serial.begin(115200);
  delay(10);

  // Khai bao GPIO5
  pinMode(up_pin, OUTPUT);
  digitalWrite(up_pin, 0);

  pinMode(down_pin, OUTPUT);
  digitalWrite(down_pin, 0);
  
  pinMode(left_pin, OUTPUT);
  digitalWrite(left_pin, 0);
          
  pinMode(right_pin, OUTPUT);
  digitalWrite(right_pin, 0);

  // Ket noi toi mang WiFi 
  Serial.println();
  Serial.println();
  Serial.print("Dang ket noi toi mang ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Da ket noi WiFi");

  // Khoi dong server
  server.begin();
  Serial.println("Khoi dong Server");

  // In dia chi IP
  Serial.println(WiFi.localIP());
}

void loop() {

  // Kiem tra neu co client ket noi
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Doi client gui data
  Serial.println("Co client moi");
  while(!client.available()){
    delay(1);
  }

  // Doc yeu cau tu client
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Kiem tra yeu cau la gi
  if (req.indexOf("/up") != -1){
    digitalWrite(up_pin, 1);
    Serial.println("up");
  }
  else if (req.indexOf("/down") != -1) {
    digitalWrite(down_pin, 1);
    Serial.println("down");
  }
  else if (req.indexOf("/left") != -1) {
    digitalWrite(left_pin, 1);
    Serial.println("left");
  }
  else if (req.indexOf("/right") != -1) {
    digitalWrite(right_pin, 1);
    Serial.println("right");
  }

  client.flush();

  // Chuan bi thong tin phan hoi
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  s += "<head>";
  s += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  s += "<script src=\"https://code.jquery.com/jquery-2.1.3.min.js\"></script>";
  s += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap.min.css\">";
  s += "</head>";
  s += "<div class=\"container\">";
  s += "<h1>Controller</h1>";
  s += "<div class=\"row\">";
  s += "<div class=\"col-md-1\"><input type=\"button\" value=\"up\" onclick=\"up()\"></div>";
  s += "<div class=\"col-md-1\"><input type=\"button\" value=\"down\" onclick=\"down()\"></div>";
  s += "<div class=\"col-md-1\"><input type=\"button\" value=\"left\" onclick=\"left()\"></div>";
  s += "<div class=\"col-md-1\"><input type=\"button\" value=\"right\" onclick=\"right()\"></div>";
  s += "</div></div>";
//  s += "</div>";
  s += "<script>function up() {$.get(\"/up\");}</script>";
  s += "<script>function down() {$.get(\"/down\");}</script>";
  s += "<script>function left() {$.get(\"/left\");}</script>";
  s += "<script>function right() {$.get(\"/right\");}</script>";

  // Gui thong tin cho client
  client.print(s);
  delay(1);
  Serial.println("Ngat ket noi Client");

}
