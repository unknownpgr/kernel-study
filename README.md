# Linux Kernel Study

- 리눅스 커널을 공부해보자.

리눅스 커널을 공부하기 위해 제일 먼저 해야 할 일은 커널 스페이스에 접근하는 일이다. 그러기 위해 loadable kernel module을 만들어보자.

- Kernel, 혹은 kernel module에서 global variable을 정의하면 data segment에 저장된다.

  - Understanding the Linux Kernel, 3rd, CHAPTER 20 Program Execution
  - ```
    vagrant@ubuntu-focal:/vagrant$ sudo cat /dev/custom_device0
    device_data_addr: 0000000031a8d5ab
    local_variable_addr: 0000000044adac09

    vagrant@ubuntu-focal:/vagrant$ sudo cat /dev/custom_device0
    device_data_addr: 0000000031a8d5ab
    local_variable_addr: 000000005d1a9995

    vagrant@ubuntu-focal:/vagrant$ sudo cat /dev/custom_device0
    device_data_addr: 0000000031a8d5ab
    local_variable_addr: 00000000ca333e7c

    vagrant@ubuntu-focal:/vagrant$ sudo cat /dev/custom_device0
    device_data_addr: 0000000031a8d5ab
    local_variable_addr: 000000005d1a9995

    vagrant@ubuntu-focal:/vagrant$
    ```

  - Local variable의 주소는 당연히 매번 바뀐다. 그러나 global variable의 주소는 항상 같다.
  - 심지어 커널모듈을 `rmmod`로 제거하고 다시 `insmod`로 로드해도 global variable의 주소는 같다.

- Kernel module의

## References

- https://olegkutkov.me/2018/03/14/simple-linux-character-device-driver/
- https://www.kernel.org/doc/Documentation/kbuild/modules.txt
- https://static.lwn.net/images/pdf/LDD3/ch03.pdf
- https://archive.kernel.org/oldlinux/htmldocs/kernel-api/API-cdev-add.html
- https://www.kernel.org/doc/html/v5.4/index.html
