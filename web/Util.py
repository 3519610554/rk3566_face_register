import time
import hashlib
import string

BASE62_CHARS = string.digits + string.ascii_lowercase + string.ascii_uppercase

#将整数转换为 Base62 编码字符串
def to_base62(value: int) -> str:
    if value == 0:
        return BASE62_CHARS[0]

    result = ""
    while value > 0:
        result = BASE62_CHARS[value % 62] + result
        value //= 62
    return result

#获取当前时间哈希Base62值
def generate_time_hash_string() -> str:
    # 获取当前时间戳（毫秒）
    millis = int(time.time() * 1000)

    # 哈希：使用 SHA256 模拟 C++ std::hash 效果（可以改成其他）
    hash_obj = hashlib.sha256(str(millis).encode())
    hash_int = int.from_bytes(hash_obj.digest(), byteorder='big')

    # Base62 编码
    return to_base62(hash_int)
