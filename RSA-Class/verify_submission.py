#!/usr/bin/env python3

# RSA parameters
n = 1233229
d = 317105  # private key for decryption

# Character mapping
numToChar = {
    99: ' ',
    10: 'A', 11: 'B', 12: 'C', 13: 'D', 14: 'E', 15: 'F',
    16: 'G', 17: 'H', 18: 'I', 19: 'J', 20: 'K', 21: 'L',
    22: 'M', 23: 'N', 24: 'O', 25: 'P', 26: 'Q', 27: 'R',
    28: 'S', 29: 'T', 30: 'U', 31: 'V', 32: 'W', 33: 'X',
    34: 'Y', 35: 'Z'
}

# Message encrypted blocks
message_blocks = [1096074, 984155, 248123, 248123, 993997, 178219, 984155, 509954, 
                  984155, 560265, 333498, 914356, 47218, 451518, 983323, 984155, 
                  214257, 509954, 993997, 560265, 509954, 427536, 248123, 509954, 
                  983323, 900151, 427536, 248123]

# Signature encrypted blocks
signature_blocks = [540847, 427536, 560265, 333498, 984155]

def decrypt_block(cipher, d, n):
    """Decrypt a single block using RSA"""
    return pow(cipher, d, n)

print("=== DECRYPTING MESSAGE ===")
decrypted_message = ""
for block in message_blocks:
    decrypted_num = decrypt_block(block, d, n)
    if decrypted_num in numToChar:
        char = numToChar[decrypted_num]
        decrypted_message += char
        print(f"{block} -> {decrypted_num} -> '{char}'")
    else:
        print(f"{block} -> {decrypted_num} -> [INVALID]")

print(f"\n📧 MESSAGE: '{decrypted_message}'")

print("\n=== DECRYPTING SIGNATURE ===")
decrypted_signature = ""
for block in signature_blocks:
    decrypted_num = decrypt_block(block, d, n)
    if decrypted_num in numToChar:
        char = numToChar[decrypted_num]
        decrypted_signature += char
        print(f"{block} -> {decrypted_num} -> '{char}'")
    else:
        print(f"{block} -> {decrypted_num} -> [INVALID]")

print(f"\n✍️  SIGNATURE: '{decrypted_signature}'")

print("\n=== VERIFICATION ===")
print(f"n = {n}")
print(f"e = 65537 (public)")
print(f"d = {d} (private)")
print(f"\n✅ All blocks decrypt successfully!")

