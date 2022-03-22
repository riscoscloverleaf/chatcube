from django.db.models import AutoField

class UnsignedAutoField(AutoField):
    def db_type(self, connection):
        return 'int(11) unsigned not null auto_increment'

    def rel_db_type(self, connection):
        return 'int(11) unsigned not null'
