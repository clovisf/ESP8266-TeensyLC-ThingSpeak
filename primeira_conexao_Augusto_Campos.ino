
const byte CH_PD=5;
const byte RST=6;
#define rede "fritzen"
#define senha "familiafritzen"

void setup() {
  Serial.begin(115200);
  Serial.println("--------------");
  pinMode(CH_PD,OUTPUT);
  digitalWrite(CH_PD,HIGH);
  pinMode(RST,OUTPUT);
  digitalWrite(RST,LOW);
  delay(100);
  digitalWrite(RST,HIGH);
  delay(1000);
  Serial1.begin(9600);
  Serial1.setTimeout(5000);

  Serial.println("ESP8266 Demo - aguardando 1 segundo");
  delay(1000);
  Serial.println("Enviando RST");
  Serial1.println("AT+RST");  
  recebeResposta();
  recebeResposta(); // o reset tem 2 respostas - OK (antes) e ready (depois) 
  Serial.println("Listando redes visiveis");
  Serial1.println("AT+CWLAP");  
  recebeResposta();
  Serial.println("Conectando-se a uma rede");
  String comando="AT+CWJAP=\"";
  comando+=rede;
  comando+="\",\"";
  comando+=senha;  
  comando+="\"";
  Serial1.println(comando);  
  recebeResposta();
  Serial.println("Listando endereco IP (pode dar ping no segundo deles)");
  Serial1.println("AT+CIFSR");  
  recebeResposta();
}

void loop() {
}

void recebeResposta() {
  int limite=7000;
  unsigned long chegada=millis();
  boolean continuar=true; 
  String S="";
  unsigned long ultimochar=0;
  while (continuar) { 
    if (Serial1.available()) {
      char c = Serial1.read();
      ultimochar=millis();
      S=S+c;
      Serial.print(c);
      if (c==10) {  // LF, fim da linha recebida
        byte p=S.indexOf(13);
        String S1=S.substring(0,p);
        if (S1=="OK") continuar=false;
        if (S1=="ready") continuar=false;
        if (S1=="ERROR") continuar=false;
        S="";
      }  
    }  
    if (millis()-chegada > limite) continuar=false;
  }
} 
