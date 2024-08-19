### 게임 로직 관련
- [X] 유저 조작
    - [X] pause
    - [X] hold
    - [X] move left/right/down
    - [X] rotate cw/ccw
    - [X] harddrop
    - [X] replay

- [X] 타이머 보정
    - [X] 하드드랍 후 바로 put
    - [X] move 후 바로 put 되는 경우 방지

- [X] 회전 보정
    - [X] wall-kick
    ~~- [ ] t-spin~~

- [X] 테트로미노 관련
    - [X] 7-bag
    - [X] nextBlocks
    - [X] ghostBlock
    - [X] drop 속도 조금씩 증가

- [X] 게임 정보 표시
    - [X] holdedBlocks canHold 여부 보여주기
    - [X] 플레이시간 보여주기
    - [X] 줄 부순 개수 보여주기
    - [X] pause, gameover 문구 표시
    - [X] 조작법 보여주기

### 애니메이션
- [X] break Full Row 관련
    - [X] 여러줄 꽉찼을 때 하나씩 부수기
    - [X] 꽉찬줄 하이라이트
    - [X] 진동효과
    - [X] 애니메이션 중 harddrop되면 바로 애니메이션 종료

- [X] replay, start 애니메이션
    - [X] border 무지개

- [X] harddrop 시각효과
    - [X] 색깔 변화 효과

- [X] 출력 최적화, 화면 깜빡임 제거


### 코드 관련
- [ ] draw함수 const 추가. lazyPrinter는 mutable로 선언
- [ ] setxy 함수 추가. grid / pixel 중 선택할 수 있게 하기
~~- [ ] const char KEY 사용자 지정 키 중복방지 static_assert~~
- [X] tui관련 상수들이랑 ROWS 대소관계 static_assert
- [ ] 애니메이션 시간 관련 상수들 sleepTime보다 큰지 static_assert
- [ ] border 두께 상수로 선언하기

- [ ] src폴더 만들고 코드관련 파일 전부 집어넣기