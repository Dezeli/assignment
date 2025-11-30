# LKM 튜토리얼 – ioctl(_IO) 연습

이 프로젝트는 System Programming 과목의 **커널 모듈 + ioctl 튜토리얼**입니다.

목표는 다음과 같습니다.

- 간단한 Linux 커널 모듈을 빌드해서 로드하고,
- `/dev/tutorial` 캐릭터 디바이스를 만든 뒤,
- `_IO` 타입 ioctl 한 번을 user-space에서 호출해 보고,
- `dmesg`로 커널 로그를 확인하는 것.

실제 과제용 프로젝트는 **별도 프로젝트**로 제공되며,  
이 레포는 **_IO 한 개만 있는 튜토리얼용 예제**라고 생각하면 됩니다.

---

## 1. 디렉토리 구조

```text
lkm-tutorial/
  .build/             # 빌드 산출물 (Make로 생성)
  include/
    abi.h             # 커널/유저 공용 ABI 정의 (디바이스 이름, ioctl 번호 등)
  kernel/
    tutorial_kernel.c # 커널 모듈 구현
    Makefile          # 커널 모듈 빌드 스크립트
  user/
    main.c            # user-space 테스트 프로그램
  Makefile            # 루트 빌드 스크립트
```

역할 요약:

- `include/abi.h`
  - `DEVICE_NAME`, `DEVICE_PATH`, `CLASS_NAME`
  - `IOCTL_MAGIC`
  - `IOCTL_HELLO` (`_IO` 타입, 인자 없음) 정의

- `kernel/tutorial_kernel.c`
  - char device 등록 (`alloc_chrdev_region`, `cdev_add`)
  - `class_create` / `device_create` 로 `/dev/tutorial` 생성
  - `open`, `release`, `unlocked_ioctl` 구현
  - `IOCTL_HELLO` 호출 시 커널 로그에 인사 메시지 출력

- `user/main.c`
  - `/dev/tutorial` 을 `open()`
  - `ioctl(fd, IOCTL_HELLO)` 호출
  - 간단한 상태 메시지 출력

---

## 2. 환경 준비 (권장)

**반드시 VM (Ubuntu)에서 작업하는 것을 권장합니다.**

이 튜토리얼은 아래 환경을 기준으로 작성·테스트되었습니다.

- Ubuntu 24.04 (VM)
- Linux 커널 **6.8.0-88-generic**

현재 커널 버전은 다음 명령으로 확인할 수 있습니다.

```bash
uname -r
# 예: 6.8.0-88-generic
```

VM 안에서 필요한 패키지를 설치합니다.

```bash
sudo apt update
sudo apt install -y build-essential gcc make linux-headers-$(uname -r)
```

> **주의**
> - `linux-headers-$(uname -r)` 가 설치되지 않으면 커널 모듈 빌드/insmod가 실패합니다.
> - 이 튜토리얼에서 사용하는 일부 커널 API(예: `class_create()` 시그니처)는
>   커널 버전에 따라 형태가 다를 수 있습니다.  
>   대략 Linux 6.4 근처에서 인자 형태가 변경되었으므로,
>   더 오래된 커널을 사용하는 경우에는 해당 커널 버전의 헤더/문서를 참고하여
>   함수 호출 형태를 맞춰야 할 수 있습니다.

---

## 3. 빌드 절차

프로젝트 루트에서:

```bash
make
```

성공하면:

- 커널 모듈: `./.build/tutorial_kernel.ko`
- 유저 프로그램: `./.build/tutorial_user`

가 생성됩니다.

---

## 4. 커널 모듈 로드 및 /dev/tutorial 확인

### 4-1. 모듈 로드 (`insmod`)

```bash
sudo insmod .build/tutorial_kernel.ko
```

에러 없이 끝나면 모듈이 커널에 올라간 상태입니다.

### 4-2. 디바이스 노드 확인

```bash
ls -l /dev/tutorial
```

대략 이런 형태의 출력이 나오면 정상입니다:

```text
crw-rw---- 1 root root <major>, <minor> /dev/tutorial
```

### 4-3. 커널 로그 확인 (`dmesg`)

```bash
sudo dmesg | tail
```

모듈이 로드되면서 비슷한 로그가 찍혀 있어야 합니다:

```text
tutorial_kernel: module loaded (major=..., minor=...)
```

---

## 5. ioctl 호출 테스트 (user-space)

### 5-1. 유저 프로그램 실행

```bash
sudo ./.build/tutorial_user
```

예상 동작:

- `/dev/tutorial` 을 열고 (`open`)
- `IOCTL_HELLO` (`_IO`) ioctl을 한 번 호출
- “dmesg를 확인하라”는 식의 메시지를 출력

### 5-2. 커널 로그에서 결과 확인

```bash
sudo dmesg | tail
```

아래와 비슷한 로그가 보이면 end-to-end 경로가 정상입니다:

```text
tutorial_kernel: device opened
tutorial_kernel: Hello from tutorial kernel module! :)
tutorial_kernel: device closed
```

---

## 6. 모듈 언로드 및 정리

### 6-1. 모듈 제거 (`rmmod`)

```bash
sudo rmmod tutorial_kernel
```

다시 `dmesg`를 확인해 보면:

```bash
sudo dmesg | tail
```

예: 

```text
tutorial_kernel: module unloaded
```

그리고 `/dev/tutorial` 도 사라져야 합니다:

```bash
ls -l /dev/tutorial
# → No such file or directory
```

### 6-2. 빌드 산출물 정리

```bash
make clean
```

- 커널 빌드 중간 파일
- `.build/` 디렉토리 전체

가 삭제됩니다. (이미 insmod 된 모듈은 `rmmod`로 따로 내린 뒤 clean 해야 합니다.)

---

## 7. 이 튜토리얼에서 확인해야 할 것

이 프로젝트는 **오직 `_IO` ioctl 한 개만**을 다룹니다.  
튜토리얼을 통해 다음 개념을 한 번씩 직접 확인해 보세요.

- `alloc_chrdev_region` → `cdev_add` → `class_create` / `device_create` 순서
- `/dev/tutorial` 에 대한 `file_operations` 연결 구조
- `ioctl()` 호출이 `unlocked_ioctl` 콜백으로 들어와서
  - 커널에서 `pr_info` 로 로그를 찍고
  - `dmesg`에서 그 결과를 확인하는 흐름