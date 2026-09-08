# v1.1.0

## what's new

change "Double-Reset-Detection" to "Multi-Reset-Detection"
Now you can configure that it needs more than 2 restarts within given timeout

```c++
MRD32(uint32_t timeout, uint32_t requiredResets)
```


## changelog

- [CHANGE] remove DRD function and replace with new MRD