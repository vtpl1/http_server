from selenium import webdriver
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions
from selenium.common.exceptions import TimeoutException
from selenium.common.exceptions import WebDriverException
from selenium.webdriver.common.by import By
from selenium.webdriver.firefox.service import Service

with webdriver.Firefox() as driver:
    try:
        url = "http://localhost:8080/static_html/index_1x1.html"

        options = webdriver.FirefoxOptions()
        options.set_capability("loggingPrefs", {'browser': 'ALL'})
        options.set_preference('devtools.console.stdout.content', True)
        service = Service(executable_path="D:\WorkFiles\http_server\geckodriver.exe")
        driver = webdriver.Firefox(service=service, options=options)

        # Open URL
        driver.get(url)
        # driver.maximize_window()

        for entry in driver.get_log('browser'):
            print(entry)

        # Setup wait for later
        wait = WebDriverWait(driver, 10000)

        # # Store the ID of the original window
        # original_window = driver.current_window_handle

        # # Check we don't have other windows open already
        # assert len(driver.window_handles) == 1

        # Click the link which opens in a new window
        # driver.find_element(By.LINK_TEXT, "new window").click()
        num_tabs = 15
        for i in range(1, num_tabs):
            driver.execute_script("window.open('http://localhost:8080/static_html/index_1x1.html')")
            wait.until(expected_conditions.title_is("Videonetics Simple Live Streaming"))

        # Wait for the new window or tab
        wait.until(expected_conditions.number_of_windows_to_be(num_tabs))

        # Loop through until we find a new window handle
        for window_handle in driver.window_handles:
            driver.implicitly_wait(1)
            driver.switch_to.window(window_handle)
            search_button = driver.find_element(by=By.NAME, value="play")
            search_button.click()
        wait.until(expected_conditions.number_of_windows_to_be(0))
    except TimeoutException as e:
        print("TimeoutException: ", e)
    except WebDriverException as e:
        print("WebDriverException: ", e)

    print("driver quitting")
    driver.quit()