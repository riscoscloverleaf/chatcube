# Generated by Django 2.2.9 on 2020-02-10 13:25

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('ik', '0007_auto_20200210_1356'),
    ]

    operations = [
        migrations.AlterField(
            model_name='chat',
            name='id',
            field=models.BigAutoField(primary_key=True, serialize=False, unique=True),
        ),
    ]
