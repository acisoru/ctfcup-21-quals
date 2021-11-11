#!/bin/bash

/waitforit.sh chouse:8123 -t 120 && php /var/www/html/init.php && rm /var/www/html/init.php && apache2-foreground