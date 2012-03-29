#ifndef _CIRC_BUFFER_H_
#define _CIRC_BUFFER_H_


#ifndef CIRC_BUFFER_SIZE
#define CIRC_BUFFER_SIZE 1028
#endif

typedef struct {
  uint16_t start;
  uint16_t end;
  uint16_t num;
  uint16_t size;
  uint8_t buf[CIRC_BUFFER_SIZE];
}circ_buffer_t;

void circ_buffer_init(circ_buffer_t *b);
bit circ_buffer_put_byte(circ_buffer_t *b, uint8_t byte);
bit circ_buffer_get_byte(circ_buffer_t *b, uint8_t *byte);

// @param bytes: the bytes going in or out
// @param n: the number of bytes requested[in], the actual number got [out]
// @return fail if n[out] < n[in]
bit circ_buffer_put_nbytes(circ_buffer_t *b, uint8_t *bytes, uint16_t *n);
bit circ_buffer_get_nbytes(circ_buffer_t *b, uint8_t *bytes, uint16_t *n);

#endif /* _CIRC_BUFFER_H_ */
