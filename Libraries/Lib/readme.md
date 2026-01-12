# 누락된 라이브러리 파일
---
## Jolt.lib
github의 업로드 용량 제한에 따라 Jolt 5.5.0 버전의 라이브러리 빌드파일을 첨부하지 못함.
cmake_vs2022_cl.bat을 통해 생성된 솔루션의 Debug, Release 두 가지 빌드의 설정을 다음과 같이 맞춰 각각 Jolt/Debug, Jolt/Release 에 넣어 사용해야함.

### 공통
**구성속성 - 일반**
- Windows SDK 버전
10.0
- C++ 언어 표준
ISO C++20 표준

### Debug
**C/C++ - 코드 생성**
- 런타임 라이브러리
다중 스레드 디버그 DLL

### Release
**C/C++ - 코드 생성**
- 런타임 라이브러리
다중 스레드 DLL