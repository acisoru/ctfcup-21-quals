FROM php:8-apache

COPY composer.json /var/www/

COPY --from=composer:latest /usr/bin/composer /usr/bin/composer

RUN apt update && apt install -y git

WORKDIR /var/www

RUN composer install

COPY public /var/www/html/

RUN a2enmod rewrite

COPY entry.sh /entry.sh
COPY waitforit.sh /waitforit.sh

ENTRYPOINT /entry.sh


