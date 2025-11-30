# Hello Kernel – 과제용 README 

이 리포지토리는 System Programming 과목 **Hello Kernel 과제**용 코드입니다.  
이 파일은 빌드 및 커널 모듈 로드/실행/제거 절차만 간단히 정리한 문서입니다.

> 커널 모듈은 반드시 **VM(연습용 리눅스 환경)** 에서 실습하는 것을 권장합니다.

---

## 1. 빌드

프로젝트 루트 디렉토리에서:

```bash
make
```

성공하면 다음 파일들이 생성됩니다 (예시):

- 커널 모듈: `./.build/hello_kernel.ko`
- 유저 프로그램: `./.build/hello_user`

---

## 2. 커널 모듈 로드

빌드가 끝난 뒤, 커널에 모듈을 올립니다.

```bash
sudo insmod .build/hello_kernel.ko
```

모듈이 정상적으로 로드되었는지 확인하려면:

```bash
sudo dmesg | tail
```

`module loaded` 와 비슷한 메시지가 출력되는지 확인합니다.

---

## 3. 디바이스 노드 확인

모듈이 `/dev/hello_kernel` 같은 캐릭터 디바이스를 생성했다면:

```bash
ls -l /dev/hello_kernel
```

대략 아래와 비슷한 출력이 보이면 정상입니다.

```text
crw-rw---- 1 root root <major> <minor> /dev/hello_kernel
```

(실제 디바이스 이름은 코드/ABI에 따라 다를 수 있습니다.)

---

## 4. 유저 프로그램 실행

유저 프로그램은 빌드 후 `.build/hello_user` 에 생성된다고 가정합니다.

```bash
sudo ./.build/hello_user
```

- 프로그램이 커널 모듈에 `ioctl()` 또는 기타 요청을 전송합니다.
- 자세한 동작은 과제 설명 및 소스 코드를 참고하세요.

실행 후 커널 로그를 다시 확인합니다.

```bash
sudo dmesg | tail
```

- 유저 프로그램에서 보낸 요청에 대응하는 커널 측 로그가 찍혀 있는지 확인합니다.

---

## 5. 커널 모듈 언로드

실습이 끝나면 커널에서 모듈을 제거합니다.

```bash
sudo rmmod hello_kernel
sudo dmesg | tail
```

- `module unloaded` 와 비슷한 메시지가 출력되는지 확인합니다.
- `/dev/hello_kernel` 디바이스 노드도 사라져야 합니다.

```bash
ls -l /dev/hello_kernel
# → No such file or directory
```

---

## 6. 빌드 산출물 정리

필요하다면 빌드 산출물을 모두 지울 수 있습니다.

```bash
make clean
```

- 커널 빌드 중간 파일
- `.build/` 디렉토리

가 제거됩니다. (이미 로드된 모듈은 `sudo rmmod hello_kernel`으로 먼저 내린 뒤 `make clean` 을 실행하세요.)

<!-- ## Optional. _IOWR 명세는 이 아래에 작성할 것 -->