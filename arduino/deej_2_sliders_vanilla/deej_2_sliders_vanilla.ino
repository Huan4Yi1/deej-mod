const int NUM_SLIDERS = 2;
const int analogInputs[NUM_SLIDERS] = {A0, A1};
const int WINDOW_SIZE = 5; // 窗口大小

int analogSliderValues[NUM_SLIDERS];
float ewmaValues[NUM_SLIDERS] = {0.0, 0.0}; // 用于存储EWMA滤波后的值
int windowBuffer[NUM_SLIDERS][WINDOW_SIZE]; // 用于存储窗口数据
int windowIndex = 0; // 当前窗口索引

float alpha = 0.08; // 滤波系数，0 < alpha < 1，越大响应越快，但噪声也越大

void setup() { 
  for (int i = 0; i < NUM_SLIDERS; i++) {
    pinMode(analogInputs[i], INPUT);
    for (int j = 0; j < WINDOW_SIZE; j++) {
      windowBuffer[i][j] = 0; // 初始化窗口缓冲区
    }
  }

  Serial.begin(115200);
}

void loop() {
  updateSliderValues();
  sendSliderValues(); // 实时发送数据
  //sendSliderValues2();
  delay(10);
}

void updateSliderValues() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
    int rawValue = analogRead(analogInputs[i]);

    // 指数加权移动平均滤波
    ewmaValues[i] = alpha * rawValue + (1 - alpha) * ewmaValues[i];

    // 将EWMA滤波后的值存储到窗口缓冲区
    windowBuffer[i][windowIndex] = ewmaValues[i];

    // 计算窗口内的平均值
    float sum = 0.0;
    for (int j = 0; j < WINDOW_SIZE; j++) {
      sum += windowBuffer[i][j];
    }
    analogSliderValues[i] = sum / WINDOW_SIZE + 1;
  }

  // 更新窗口索引
  windowIndex = (windowIndex + 1) % WINDOW_SIZE;
}

void sendSliderValues() {
  String builtString = "";
  for (int i = 0; i < NUM_SLIDERS; i++) {
    builtString += String((int)analogSliderValues[i]);
    if (i < NUM_SLIDERS - 1) {
      builtString += "|";
    }
  }
  Serial.println(builtString);
}

void sendSliderValues2() {
  String builtString = "";
  for (int i = 0; i < NUM_SLIDERS; i++) {
    float normalizedValue = analogSliderValues[i] / 1023.0; // 将值标准化到0到1的范围
    builtString += String(normalizedValue, 3); // 打印3位小数
    if (i < NUM_SLIDERS - 1) {
      builtString += "|";
    }
  }
  Serial.println(builtString);
}
