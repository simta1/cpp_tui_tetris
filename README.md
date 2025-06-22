# cpp_tui_tetris
콘솔창에서 구현한 테트리스   
지솦 SPC 제출용   
윈도우11에서 만듦   
컴파일 후 main.exe 실행
```
make
```
make 설치가 안 되어 있다면
```
g++ src/main.cpp -O2 -std=c++17 -o main.exe
```   

## 조작법
src/userControlKey.h 에서 원하는 키로 수정 가능

| 기능                | 키               |
|---------------------|-------------------|
| 게임 일시정지        | `p`              |
| (게임오버 시) 재시작 | `r`              |
| 왼쪽 이동            | `a`              |
| 오른쪽 이동          | `d`              |
| 아래 이동            | `s`              |
| 반시계방향 회전      | `←` (좌측 화살표) |
| 시계방향 회전        | `→` (우측 화살표) |
| hard drop            | `↑` (위쪽 화살표)|
| 홀드                 | `Space`          |
| 디자인 테마 변경     | `t`              |

## 주의사항
노트북에서 실행할 경우 충전기 꽂은 상태로 실행해야 함   
아닐 경우 애니메이션 속도 감소 꽤 심함

## 게임화면
#### 테마 1
![cpp_tui_tetris_thema1](https://github.com/user-attachments/assets/a1917b75-43cd-491f-9478-56c3dc9d53fd)

#### 테마 2
![cpp_tui_tetris_thema2](https://github.com/user-attachments/assets/42623739-8ab1-4341-bce1-796bc747b9c9)

### 플레이영상
![tetris_play](https://github.com/user-attachments/assets/0aa21463-dfb8-42bb-93ab-4887a0f3118f)

## 시각효과
- 채워진 줄 부수는 애니메이션
    1. 부서질 줄 강조표시와 함께 화면진동
    2. 부서질 줄이 여러개라면 윗쪽부터 하나씩 제거
    3. 애니메이션이 끝나기 전에 새로운 hard drop이 입력될 경우 애니메이션 종료 후 즉시 줄을 부순 뒤 hard drop 진행

- hard drop 애니메이션
    1. hard drop이후 줄이 부서진다면 테트로미노가 떨어진 충돌 지점을 중심으로 원모양의 하양색 충격파 발생
    2. 충격파의 크기는 hard drop의 시작높이에 비례
    3. 충격파를 일으키기 위한 최소 높이 필요

- 무지개 애니메이션
    1. 한번에 3개 이상의 줄을 부순 경우 게임보드 테두리에 무지개빛 점멸 발생