# Generated by Django 2.2.6 on 2020-01-29 17:26

from django.conf import settings
from django.db import migrations, models
import django.db.models.deletion
import encrypted_fields.fields


class Migration(migrations.Migration):

    initial = True

    dependencies = [
        ('ik', '0001_initial'),
    ]

    operations = [
        migrations.CreateModel(
            name='TelegramAccount',
            fields=[
                ('member', models.OneToOneField(on_delete=django.db.models.deletion.CASCADE, primary_key=True, related_name='telegram_account', serialize=False, to=settings.AUTH_USER_MODEL)),
                ('_phone_data', encrypted_fields.fields.EncryptedCharField(max_length=22)),
                ('phone', encrypted_fields.fields.SearchField(db_index=True, encrypted_field_name='_phone_data', hash_key='21a90509d5098703462a23319f8cec11', max_length=66, null=True, unique=True, verbose_name='Phone')),
                ('tg_user_id', models.BigIntegerField(default=0)),
            ],
            options={
                'db_table': 'telegram_accounts',
            },
        ),
    ]
