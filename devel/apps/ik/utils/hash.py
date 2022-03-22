import hashlib


def compute_file_hash(file_or_path):
    file_pos = 0
    if hasattr(file_or_path, 'read'):
        needs_close = file_or_path.closed
        file = file_or_path
        if not file_or_path.closed:
            file_pos = file.tell()
            file.seek(0)
        else:
            file = open(file_or_path.name, 'rb')
    else:
        file = open(file_or_path, 'rb')
        needs_close = True

    crc = hashlib.md5()
    try:
        chunk_size = 65535
        while 1:
            data = file.read(chunk_size)
            if not data:
                break
            crc.update(data)
    finally:
        if needs_close:
            file.close()
        else:
            file.seek(file_pos)

    return crc.hexdigest()


def get_member_by_hash(val):
    hash1 = val[:20]
    hash_with_id = val[20:]
    end_id = hash_with_id.index("f")
    id = int(hash_with_id[:end_id])
    hash = hash1 + hash_with_id[end_id+1:]
    member = Member.objects.get(pk=id)

