FROM debian:buster-slim

RUN apt-get update && apt-get install -yq gnupg curl
RUN curl -fsSL https://dl.google.com/linux/linux_signing_key.pub | apt-key add - && \
    echo "deb http://dl.google.com/linux/chrome/deb/ stable main" >> /etc/apt/sources.list && \
    # Cypress dependencies
    apt-get update && apt-get install -yq google-chrome-stable

ENTRYPOINT [ "/usr/bin/google-chrome", \
    "--headless", \
    "--no-sandbox", \
    "--disable-gpu", \
    "--remote-debugging-address=0.0.0.0", \
    "--remote-debugging-port=9222", \
    "--use-mock-keychain", \
    "--password-store=basic", \
    "--disable-dev-shm-usage", \
    "--test-type=webdriver", \
    "--enable-automation", \
    "--disable-hang-monitor", \
    "--window-size=1280,720", \
    "--disable-background-networking", \
    "--disable-default-apps", \
    "--disable-extensions", \
    "--disable-sync", \
    "--disable-web-resources", \
    "--disable-notifications", \
    "--disable-translate", \
    "--hide-scrollbars", \
    "--metrics-recording-only", \
    "--mute-audio", \
    "--no-first-run", \
    "--safebrowsing-disable-auto-update", \
    "--user-data-dir=/tmp" \
    ]
