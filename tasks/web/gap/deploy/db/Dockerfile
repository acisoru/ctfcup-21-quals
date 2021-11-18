FROM amazon/opendistro-for-elasticsearch:1.13.2

COPY config.yml /usr/share/elasticsearch/plugins/opendistro_security/securityconfig/config.yml
COPY internal_users.yml /usr/share/elasticsearch/plugins/opendistro_security/securityconfig/internal_users.yml
COPY roles_mapping.yml /usr/share/elasticsearch/plugins/opendistro_security/securityconfig/roles_mapping.yml
COPY roles.yml /usr/share/elasticsearch/plugins/opendistro_security/securityconfig/roles.yml