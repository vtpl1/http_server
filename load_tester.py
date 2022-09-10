import logging
from selenium import webdriver
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions
from selenium.common.exceptions import TimeoutException
from selenium.common.exceptions import WebDriverException
from selenium.webdriver.common.by import By
from selenium.webdriver.chrome.service import Service


def get_browser_log_entries(driver, file):
    """get log entreies from selenium and add to python logger before returning"""
    loglevels = {
        'NOTSET': 0,
        'DEBUG': 10,
        'INFO': 20,
        'WARNING': 30,
        'ERROR': 40,
        'SEVERE': 40,
        'CRITICAL': 50
    }

    #initialise a logger
    browserlog = logging.getLogger("chrome")
    #get browser logs
    slurped_logs = driver.get_log('browser')
    for entry in slurped_logs:
        #convert broswer log to python log format
        rec = browserlog.makeRecord(
            "%s.%s" % (browserlog.name, entry['source']),
            loglevels.get(entry['level']), '.', 0, entry['message'], None,
            None)
        rec.created = entry[
            'timestamp'] / 1000  # log using original timestamp.. us -> ms
        try:
            #add browser log to python log
            browserlog.handle(rec)
        except:
            print(entry)
            # file.write(entry)
    #and return logs incase you want them
    return slurped_logs


def web_simulate():
    try:
        url = "http://localhost:8080/static_html/index_1x1.html"

        options = webdriver.ChromeOptions()
        options.set_capability("goog:loggingPrefs", {'browser': 'ALL'})
        service = Service(executable_path="chromedriver.exe")
        driver = webdriver.Chrome(service=service, options=options)

        num_tabs = 5
        # # Open URL
        # driver.get(url)
        # # driver.maximize_window()

        # file = open("D:\WorkFiles\http_server\console_log.txt", "w")

        # consolemsgs = get_browser_log_entries(driver, file)

        # Setup wait for later
        wait = WebDriverWait(driver, 100)

        for i in range(0, num_tabs):
            driver.execute_script(f"window.open('about:blank', 'tab{i}');")
            driver.switch_to.window(f"tab{i}")
            driver.get(url)
            wait.until(
                expected_conditions.element_to_be_clickable((By.NAME, "play")))
            driver.find_element(by=By.NAME, value="play").click()

        # # Loop through until we find a new window handle
        # for window_handle in driver.window_handles:
        #     driver.implicitly_wait(1)
        #     driver.switch_to.window(window_handle)
        #     search_button = driver.find_element(by=By.NAME, value="play")
        #     search_button.click()
        wait.until(expected_conditions.number_of_windows_to_be(0))
    except TimeoutException as e:
        print("TimeoutException: ", e)
    except WebDriverException as e:
        print("WebDriverException: ", e)
    finally:
        print("driver quitting")
        driver.quit()


if __name__ == "__main__":
    logging.basicConfig(level=logging.DEBUG,
                        format='%(asctime)s:%(levelname)7s:%(message)s')
    logging.info("start")
    web_simulate()
    logging.info("end")