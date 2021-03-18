

void setup() {
  Serial.begin(9600);
  Serial.println(eval("text a $(hello)$ hello;"));
  Serial.println(eval("#a = 1;"));
}

void loop() {}

String eval(String com) {

  String result = "";

  int i = 0;
  int l = 0;
  String args[4];
  String cur = "";
  String c, nc;

  Serial.print("new evaluation started for ");
  Serial.println(com);
  
  while (i < com.length() && nc != ";") {
    c = com.substring(i, i + 1);
    if (i + 1 < com.length()) {
      nc = com.substring(i + 1, i + 2);
    } else {
      nc = " ";
    }
    if (c == " " || c == ";") {
      args[l] = cur;
      cur = "";
      l++;
      i++;
    } else if (c == "$" && nc == "(") {
      int next = com.indexOf(")$", i);
      args[l] = eval("$" + com.substring(i + 2, next) + ";");
      i = next + 2;
      l++;
    } else if (c == "#" && nc == "(") {
      int next = com.indexOf(")#", i);
      args[l] = eval("#" + com.substring(i + 2, next) + ";");
      i = next + 2;
      l++;
    } else if (c == "@" && nc == "(") {
      int next = com.indexOf(")@", i);
      args[l] = eval(com.substring(i + 2, next) + ";");
      i = next + 2;
      l++;
    } else {
      cur = cur + c;
      i++;
    }
  }

  String instr = args[0];
  for (int i = 0; i < l; i++) {
    Serial.print("{");
    Serial.print(com);
    Serial.print("} ");
    Serial.print(i);
    Serial.print(" ");
    Serial.println(args[i]);
  }

  if (instr == "text") {
    result = "YAY!!!";
  } else if (instr.substring(0, 1) == "$") {
    String vname = instr.substring(1, instr.length());
    if (args[1] == "==") {
      result = (readf(vname) == args[2]);
    } else {
      result = readf(vname);
    }
  } else if (instr.substring(0, 1) == "#") {
    String vname = instr.substring(1, instr.length());
    if (args[1] == "=") {
      writef(vname, args[2]);
    }
  }

  return result;

}

void writef(String n, String val) {
  Serial.print("SET");
}

String readf(String n) {
  return "success";
}
