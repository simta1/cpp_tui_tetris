### 게임 로직 관련
- [ ] 유저 조작
    - [X] pause
    - [X] hold
    - [X] move left/right/down
    - [X] rotate cw/ccw
    - [X] harddrop
    - [ ] replay

- [X] 타이머 보정
    - [X] 하드드랍 후 바로 put
    - [X] move 후 바로 put 되는 경우 방지

- [ ] 회전 보정

- [ ] 테트로미노 관련
    - [X] 7-bag
    - [ ] nextBlocks 보여주기
    - [X] ghostBlock 보여주기
    - [ ] holdedBlocks canHold 여부 보여주기

- [ ] 기타
    - [ ] 플레이시간 보여주기
    - [ ] 줄 부순 개수 보여주기

### 애니메이션
- [ ] break Full Row 관련
    - [X] 여러줄 꽉찼을 때 하나씩 부수기
    - [X] 꽉찬줄 하이라이트
    - [ ] border 시각효과
    - [ ] perfect clear 시각효과

- [ ] harddrop 시각효과
    - [X] 진동 -> 코드 짜긴 했는데 주석처리 해 둠
    - [ ] 반짝거리는 효과

- [ ] 출력 최적화, 화면 깜빡임 제거


### assert
    const char KEY 사용자 지정 부분 키 겹친 거 없나 static_assert로 확인할지?