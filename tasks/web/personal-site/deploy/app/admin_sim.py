import asyncio
import selectors

import pyppeteer


class AdminSimulator:
    CHROME_ARGS = ["--no-sandbox", "--disable-gpu",
                   "--use-mock-keychain",
                   "--password-store=basic",
                   "--disable-dev-shm-usage",
                   "--test-type=webdriver",
                   "--enable-automation",
                   "--disable-hang-monitor",
                   "--window-size=1280,650",
                   "--disable-background-networking",
                   "--disable-default-apps",
                   "--disable-extensions",
                   "--disable-sync",
                   "--disable-web-resources",
                   "--disable-notifications",
                   "--disable-translate",
                   "--hide-scrollbars",
                   "--metrics-recording-only",
                   "--mute-audio",
                   "--no-first-run",
                   "--safebrowsing-disable-auto-update",
                   "--disable-web-security"
                   "--user-data-dir=/tmp"
                   ]

    def __init__(self, host, login, password, chrome_host='', exec_path='/usr/bin/google-chrome'):
        self.host = host
        self.login = login
        self.password = password
        self.chrome_host = chrome_host
        self.exec_path = exec_path

    async def simulate_admin(self, context, uuid):
        print("CELERY: ", uuid)
        page = await context.newPage()
        page.setDefaultNavigationTimeout(2 * 1000)
        await asyncio.wait([
            page.goto(f'http://{self.host}/login'),
            page.waitForNavigation(),
        ])
        await page.type('#id_username', self.login)
        await page.type('#id_password', self.password)
        await asyncio.gather(
            page.waitForNavigation(),
            page.click("#submitButton"),
        )
        await asyncio.wait([
            page.goto(f'http://{self.host}/share/{uuid}', waitUntil='networkidle0'),
            page.waitForNavigation(),
        ])

    async def fetch_as_admin_chrome(self, uuid):
        # browser = await pyppeteer.launch({
        #     'executablePath': self.exec_path,
        #     'headless': True, 'args': self.CHROME_ARGS
        # })
        b_url = f'http://{self.chrome_host}:9222'
        browser = await pyppeteer.connect(browserURL=b_url)
        context = await browser.createIncognitoBrowserContext()
        try:
            await self.simulate_admin(context, uuid)
        except Exception as e:
            print(e)
        finally:
            await context.close()
