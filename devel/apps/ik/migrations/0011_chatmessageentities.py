# Generated by Django 2.2.9 on 2020-07-27 18:19

from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    dependencies = [
        ('ik', '0010_auto_20200531_1338'),
    ]

    operations = [
        migrations.CreateModel(
            name='ChatMessageEntities',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('type', models.IntegerField()),
                ('start', models.IntegerField()),
                ('len', models.IntegerField()),
                ('value', models.CharField(default='', max_length=255)),
                ('message', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, related_name='entities', to='ik.Message')),
            ],
            options={
                'db_table': 'chat_messages_entities',
            },
        ),
    ]
