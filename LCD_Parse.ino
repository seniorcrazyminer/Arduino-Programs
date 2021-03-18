void setup() {
  Serial.begin(9600);
  Serial.println(eval("text(0,$abc,text($a,$b,$c,12,$d),1,0)"));

}

void loop() {
  // put your main code here, to run repeatedly:

}

bool inString(String str, String sub) {
  int i = str.indexOf(sub);
  if (i == -1) {
    return false;
  } else {
    return true;
  }
}

String getNextDelim(String s, String del, int numDone) {
  int idx = 0, nextIdx;
  for (int i = 0; i < numDone; i++) {
    idx = s.indexOf(del, idx+1);
  }
  if (idx == -1) {
    return "!!!";
  } else {
    nextIdx = s.indexOf(del, idx + 1);
    if (nextIdx == -1) {
      return s.substring(idx+1, s.length());
    } else {
      return s.substring(idx + (idx != 0), nextIdx);
    }
    
  }
}

String eval(String s) {
  int cur = 0;
  String body, nam;
  String params[8];
  bool isCom;
  if (inString(s, "(")) {
    isCom = true;
    cur = s.indexOf("(");
    nam = s.substring(0, cur);
    body = s.substring(cur + 1, s.length() - 1);

    Serial.print(nam);
    Serial.print("  ");
    Serial.println(body);
    String item;
    int num=0, num1=0;

    while (item != "!!!") {
      item = getNextDelim(body, ",", num);
      if (inString(item, "(")) {
        int st = body.indexOf(item);
        int i = 0;
        int numSkips = 0;
        while(body.charAt(st + i) != ')') {
          if (body.charAt(st + i) == ',') numSkips++;
          i++;
        }
        item = body.substring(st, st+i + 1);
        num += numSkips + 1; 
      } else {
        num++;
      }
      if (item != "!!!") {
        params[num1] = item;
        Serial.print(num1); Serial.print(" "); Serial.println(item);
        num1++;
      }
    }

    for (int i = 0; i < num; i++) {
      params[i] = eval(params[i]);
      Serial.print(i); Serial.print("e "); Serial.println(params[i]);
    }

    
  } else {
    isCom = false;
    body = s;
  }

  String retrn;

  if (isCom) {
    if (nam == "text") {
      tft.txt(params[0].toInt(), params[1].toInt(), params[2], params[3].toInt(), params[4].toInt());
      retrn = params[2];
    }
  } else {
    if (body.charAt(0) == '$') {
      retrn = getVal(body.substring(1, body.length()));
    } else {
      retrn = body;
    }
  }

  return retrn;

}

String getVal(String nam) {
  return "0";
}
