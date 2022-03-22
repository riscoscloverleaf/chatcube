# Generated by Django 2.2.9 on 2020-02-10 12:56

from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    dependencies = [
        ('ik', '0001_initial'),
    ]

    operations = [
        migrations.AddField(
            model_name='member',
            name='city',
            field=models.CharField(blank=True, default='', max_length=255),
        ),
        migrations.AddField(
            model_name='member',
            name='country',
            field=models.ForeignKey(blank=True, null=True, on_delete=django.db.models.deletion.SET_NULL, to='ik.Country'),
        ),
    ]
