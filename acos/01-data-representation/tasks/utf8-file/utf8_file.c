#include "utf8_file.h"

int bytes_to_write_f(int cnt) {
  if (cnt >= 25) {
    return 1;
  } else if (cnt >= 21) {
    return 2;
  } else if (cnt >= 16) {
    return 3;
  } else {
    return 4;
  }
}

int utf8_write(utf8_file_t* f, const uint32_t* str, size_t count) {
  ssize_t result;
  int bytes_to_write;
  int success_count = 0;
  size_t i = 0;

  for (; i < count; ++i) {
    uint32_t mask = 1 << 31;
    int cnt = 0;
    while ((str[i] & mask) == 0) {
      ++cnt;
      mask >>= 1;
    }

    bytes_to_write = bytes_to_write_f(cnt);

    if (bytes_to_write == 1) {
      uint32_t mask_for_lead = 0x0000007f; // "0111 1111"
      uint8_t storage_for_lead = (uint8_t)(str[i] & mask_for_lead);
      result = write(f->fd, &storage_for_lead, 1);
      if (result < 0) {
        return -1; // TODO: SHOULD RETURN SUCCESS_CNT
      }
    } else {
      uint32_t mask_for_lead = ((1 << 7) >> bytes_to_write) - 1;
      uint32_t mask_for_lead_2 = ((1 << bytes_to_write) - 1) << (8 - bytes_to_write);
      int shift = (bytes_to_write - 1) * 6;
      uint8_t storage_for_lead = mask_for_lead_2 + (uint8_t)((str[i] & (mask_for_lead << shift)) >> shift);
      result = write(f->fd, &storage_for_lead, 1);
      if (result < 0) {
        return -1; // TODO: SHOULD RETURN SUCCESS_CNT
      }

      uint32_t mask_for_left = 63;
      for (int j = 1; j < bytes_to_write; ++j) {
        int shift_for_left = 6 * (bytes_to_write - 1 - j);
        uint8_t storage_for_left = 128 + (uint8_t)((str[i] & (mask_for_left << shift_for_left)) >> shift_for_left);
        result = write(f->fd, &storage_for_left, 1);
        if (result < 0) {
          return -1; // TODO: SHOULD RETURN SUCCESS_CNT
        }
      }
    }
    success_count += bytes_to_write;
  }
  return i;
}

int bytes_to_read_f(uint8_t buf) {
  int bytes_to_read = 0;
  uint32_t mask = 1 << 7;
  if ((buf & mask) == 0x00) {
    return ++bytes_to_read;
  } else {
    while ((buf & mask) != 0) {
      ++bytes_to_read;
      mask >>= 1;
    }
  }
  return bytes_to_read;
}

uint8_t mask_for_lead_f(uint8_t buf, int bytes_to_read) {
  uint8_t mask = 0;
  if (bytes_to_read == 1) {
    mask = 0x7f; // "0111 1111"
  } else {
    mask = ((1 << 7) >> bytes_to_read) - 1;
  }
  return mask;
}

int utf8_read(utf8_file_t* f, uint32_t* res, size_t count) {
  ssize_t result;
  int bytes_to_read;
  int success_count = 0;
  uint8_t buf = 0;
  size_t i = 0;

  for (; i < count; ++i) {
    result = read(f->fd, &buf, 1); // TODO: CHECK RESULT AND CORRECTNESS
    if (result == 0 && success_count == 0) {
      return 0;
    } else if (result == 0) {
      return i;
    } else if (result < 0) {
      return -1;
    }
    bytes_to_read = bytes_to_read_f(buf);
    if (bytes_to_read > 6) {
      return -1;
    }
    uint32_t tmp = 0;
    uint8_t mask_for_lead = mask_for_lead_f(buf, bytes_to_read);
    tmp += (buf & mask_for_lead);
    for (int j = 1; j < bytes_to_read; ++j) {
      result = read(f->fd, &buf, 1);
      if (result == 0) {
        return i;
      } else if (result < 0) {
        return -1;
      }
      // TODO: CHECK !!!
      tmp <<= 6;
      uint8_t mask_for_byte = 0x3f; // "0011 1111"
      tmp += (buf & mask_for_byte);
    }
    res[i] = tmp;
    success_count += bytes_to_read;
  }
  return i;
}

utf8_file_t* utf8_fromfd(int fd) {
  utf8_file_t* file = (utf8_file_t*)malloc(sizeof(utf8_file_t));
  file->fd = fd;
  return file;
}
