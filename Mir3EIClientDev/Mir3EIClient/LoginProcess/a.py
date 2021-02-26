shex = "37093CEE5FC868CC7DDE48EB4EFE68C96FC77587B441FE0EEE36E240DE50C151CA58C554AD4AFC0A32CD7BD025F36EB5B0"

KEYS = b"SoftSmile"
bs = bytes.fromhex(shex)

def decode(hexstr):
	rs = []
	last_b = 0
	for i, b in enumerate(bs):
		if i == 0:
			last_b = b
			continue
		k = KEYS[(i-1)%len(KEYS)]
		t = k^b
		if last_b > t: t += 0xFF
		t -= last_b
		t = t & 0xFF
		last_b = b
		rs.append(t)
	return bytes(rs)
	
def encode(s):
	last_b = 0x11
	rs = [last_b]
	for i, c in enumerate(s):
		t = ord(c)
		t += last_b
		if t >= 0xFF: t-=0xFF
		k = KEYS[i%len(KEYS)]
		t &= 0xFF
		b = k^t
		rs.append(b)
		last_b = b

	return bytes(rs).hex().upper()

s = decode(shex).decode('latin_1')
print(s)
#print(encode("传奇EI3.0 电信".encode("gbk").decode('latin_1')))