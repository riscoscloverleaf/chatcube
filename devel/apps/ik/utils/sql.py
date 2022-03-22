import time
from django.db.utils import OperationalError

def update_with_retry(qs, **kwargs):
    attempts = 0
    while attempts < 3:
        try:
            return qs.update(**kwargs)
        except OperationalError as e:
            code = e.args[0]
            if attempts == 2 or code != 1213:
                raise e
            attempts += 1
            time.sleep(0.2)
