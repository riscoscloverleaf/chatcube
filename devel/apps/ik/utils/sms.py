import requests
import logging
from django.conf import settings

budget_sms_error_codes = {
'1001': 'Not enough credits to send messages',
'1002': 'Identification failed. Wrong credentials',
'1003': 'Account not active, contact BudgetSMS',
'1004': 'This IP address is not added to this account. No access to the API',
'1005': 'No handle provided',
'1006': 'No UserID provided',
'1007': 'No Username provided',
'2001': 'SMS message text is empty',
'2002': 'SMS numeric senderid can be max. 16 numbers',
'2003': 'SMS alphanumeric sender can be max. 11 characters',
'2004': 'SMS senderid is empty or invalid',
'2005': 'Destination number is too short',
'2006': 'Destination is not numeric',
'2007': 'Destination is empty',
'2008': 'SMS text is not OK (check encoding?)',
'2009': 'Parameter issue (check all mandatory parameters, encoding, etc.)',
'2010': 'Destination number is invalidly formatted',
'2011': 'Destination is invalid',
'2012': 'SMS message text is too long',
'2013': 'SMS message is invalid',
'2014': 'SMS CustomID is used before',
'2015': 'Charset problem',
'2016': 'Invalid UTF-8 encoding',
'2017': 'Invalid SMSid',
'3001': 'No route to destination. Contact BudgetSMS for possible solutions',
'3002': 'No routes are setup. Contact BudgetSMS for a route setup',
'3003': 'Invalid destination. Check international mobile number formatting',
'4001': 'System error, related to customID',
'4002': 'System error, temporary issue. Try resubmitting in 2 to 3 minutes',
'4003': 'System error, temporary issue.',
'4004': 'System error, temporary issue. Contact BudgetSMS',
'4005': 'System error, permanent',
'4006': 'Gateway not reachable',
'4007': 'System error, contact BudgetSMS',
'5001': 'Send error, Contact BudgetSMS with the send details',
'5002': 'Wrong SMS type',
'5003': 'Wrong operator',
'6001': 'Unknown error',
'7001': 'No HLR provider present, Contact BudgetSMS.',
'7002': 'Unexpected results from HLR provider',
'7003': 'Bad number format',
'7901': 'Unexpected error. Contact BudgetSMS',
'7902': 'HLR provider error. Contact BudgetSMS',
'7903': 'HLR provider error. Contact BudgetSMS'
}

logger = logging.getLogger('ik')

def send_sms(number, txt):
    data = {
        'handle': "b0258a045e7415e44da38c656111210b",
        "username": "iksoftware",
        "userid": "8195",
        "to": number,
        "msg": txt,
        "from": settings.SITE_NAME,
        "price": 1,
        "credit": 1
    }

    # logger.info("SMS sent to: %s, price: %s, text: %s, response: %s", number, 0, txt, "ok")
    # return {'error': None, 'price': 0, 'credit': 0, 'response': "ok"}

    url = 'https://api.budgetsms.net/sendsms/'
    response = requests.get(url, data, verify=False)
    if response.status_code == 200:
        parts = response.text.split(' ')
        if parts[0] == 'OK':
            price = parts[2]
            credit = parts[4]
            logger.info("SMS sent to: %s, price: %s, text: %s, response: %s", number, price, txt, response.text)
            return {'error': None, 'price': price, 'credit': credit, 'response': response.text}
        elif parts[0] == 'ERR':
            logger.error("SMS NOT sent to: %s, error: %s, text: %s, response: %s", number, budget_sms_error_codes[parts[1]], txt, response.text)
            return {'error': budget_sms_error_codes[parts[1]], 'response': response.text}

    logger.error("SMS NOT sent to: %s, http status: %s, response: %s", number, response.status_code, response.text)
    return {'error': response.text, 'response': response.text}
