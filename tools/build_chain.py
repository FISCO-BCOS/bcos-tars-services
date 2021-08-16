#!/usr/bin/python
# -*- coding: UTF-8 -*-
import sys
from requests_toolbelt import MultipartEncoder
import requests
import argparse
import subprocess
import time
import os
import toml

service_list = ["StorageService", "DispatcherService", "ExecutorService",
                "FrontService", "GatewayService", "TxPoolService", "PBFTService", "RpcService"]
config_list = ["config.ini", "config.genesis", "nodes.json",
               "node.nodeid", "node.pem", "ca.crt", "ssl.key", "ssl.crt"]
tars_pkg_post_fix = ".tgz"


def log_error(error_msg):
    print("\033[31m%s \033[0m" % error_msg)


def log_info(error_msg):
    print("\033[32m%s \033[0m" % error_msg)


class DeployInfo:
    def __init__(self, deploy_info):
        self.deploy_ip = DeployInfo.get_value(
            deploy_info, "deploy_ip", None, True)
        if self.deploy_ip == "127.0.0.1":
            raise Exception("the deploy ip must not be 127.0.0.1")
        self.node_count = DeployInfo.get_value(
            deploy_info, "node_count", 1, False)

    def get_value(deploy_info, key, default_value, must_exist):
        if key not in deploy_info:
            if must_exist:
                raise Exception(
                    "the value for deploy_info.%s must be set" % key)
            return default_value
        return deploy_info[key]


class BuildChainConfig:
    def __init__(self, config):
        # set tars configuration
        self.tars_url = BuildChainConfig.get_value(
            config, "tars", "tars_url", None, True)
        self.tars_token = BuildChainConfig.get_value(
            config, "tars", "tars_token", None, True)
        self.tars_pkg_dir = BuildChainConfig.get_value(
            config, "tars", "tars_pkg_dir", None, True)
        # set chain configuration
        self.app_name = BuildChainConfig.get_value(
            config, "chain", "app_name", "nodes", False)
        self.sm_mode = BuildChainConfig.get_value(
            config, "chain", "sm_mode", False, False)
        self.p2p_listen_port = BuildChainConfig.get_value(
            config, "chain", "p2p_listen_port", 30300, False)
        self.rpc_listen_port = BuildChainConfig.get_value(
            config, "chain", "rpc_listen_port", 20200, False)
        self.leader_period = BuildChainConfig.get_value(
            config, "chain", "leader_period", 1, False)
        self.block_tx_count_limit = BuildChainConfig.get_value(
            config, "chain", "block_tx_count_limit", 1000, False)
        self.consensus_timeout = BuildChainConfig.get_value(
            config, "chain", "consensus_timeout", 3000, False)
        self.gas_limit = BuildChainConfig.get_value(
            config, "chain", "gas_limit", "300000000", False)
        # set the deploy info
        if "deploy_info" not in config:
            raise Exception("must set the deploy info")
        deploy_info_list = config["deploy_info"]
        self.deploy_info_list = []
        for item in deploy_info_list:
            deploy_info = DeployInfo(item)
            self.deploy_info_list.append(deploy_info)

    def get_value(config, section, key, default_value, throw_exception):
        if section not in config or key not in config[section]:
            if throw_exception:
                raise Exception("must set value for %s.%s" % (section, key))
            return default_value
        return config[section][key]


class TarsTool:
    'basic class to access the tars'

    def __init__(self, tars_url, tars_token, app_name, deploy_ip):
        self.tars_url = tars_url
        self.tars_token = tars_token
        self.deploy_ip = deploy_ip
        if self.tars_url.endswith('/') is False:
            self.tars_url = self.tars_url + '/'
        self.add_application_url = self.tars_url + 'api/add_application'
        self.deploy_service_url = self.tars_url + 'api/deploy_server'
        self.get_port_url = self.tars_url + 'api/auto_port'
        self.add_config_url = self.tars_url + 'api/add_config_file'
        self.update_config_url = self.tars_url + 'api/update_config_file'
        # upload and publish the package
        self.upload_package_url = self.tars_url + 'api/upload_patch_package'
        self.add_task_url = self.tars_url + 'api/add_task'
        self.get_server_list_url = self.tars_url + 'api/server_list'
        self.config_file_list_url = self.tars_url + 'api/config_file_list'

        self.app_name = app_name
        self.token_param = {'ticket': self.tars_token}

    def create_application(self):
        "create application"
        if self.app_exists() is True:
            log_error("application %s already exists" % self.app_name)
            return False
        log_info("create application: %s" % self.app_name)
        request_data = {'f_name': self.app_name}
        response = requests.post(
            self.add_application_url, params=self.token_param, data=request_data)
        return TarsTool.parse_response("create application " + self.app_name, response)

    def parse_response(operation, response):
        if response.status_code != 200:
            log_error("%s failed, error message: %s, error code: %d" %
                      (operation, response.content, response.status_code))
            return False
        result = response.json()
        error_msg = result['err_msg']
        if len(error_msg) > 0:
            log_error("%s failed, error message: %s" % (operation, error_msg))
            return False
        return True

    def get_auto_port(self):
        # get the auto_port
        log_info("get the un-occuppied port")
        params = {"node_name": self.deploy_ip, "ticket": self.tars_token}
        response = requests.get(self.get_port_url, params=params)
        if TarsTool.parse_response("get the un-occupied port", response) is False:
            return (False, 0)
        result = response.json()
        if 'data' not in result:
            log_error("get empty un-occupied port")
            return (False, 0)
        node_info = result['data']
        if len(node_info) <= 0:
            log_error("get empty un-occupied port")
            return (False, 0)
        if 'port' not in node_info[0]:
            log_error("get empty un-occupied port")
            return (False, 0)
        port = node_info[0]['port']
        log_info("get the un-occupied port success, port: %s" % (port))
        return (True, int(port))

    def deploy_single_service(self, service_name):
        "deploy single service"
        if self.server_exists(service_name) is True:
            log_error("service %s already exists." % service_name)
            return False
        # get the un-occupied port
        (ret, port) = self.get_auto_port()
        if ret is False:
            log_error("deploy service %s failed for get un-occupied port failed" %
                      service_name)
            return False
        log_info("deploy service %s" % service_name)
        adapters = [{"obj_name": service_name + "Obj", "port": port, "bind_ip": self.deploy_ip, "port_type": "tcp",
                     "thread_num": 5, "max_connections": 100000, "queuecap": 50000, "queuetimeout": 20000}]
        request_data = {"application": self.app_name, "server_name": service_name, "node_name": self.deploy_ip,
                        "server_type": "tars_cpp", "template_name": "DCache.Cache", 'adapters': adapters}
        response = requests.post(
            self.deploy_service_url, params=self.token_param, json=request_data)
        if TarsTool.parse_response("deploy service " + service_name, response) is False:
            return False
        return True

    def deploy_service_list(self, service_list):
        "deploy service list"
        for service in service_list:
            if self.deploy_single_service(service) is False:
                log_error("deploy service list failed, service list: %s" %
                          service_list)
                return False
        return True

    def add_app_config_file(self, config_file_name, config_file_path):
        "add the config file"
        log_info("add config file for application %s, config file path: %s" %
                 (self.app_name, config_file_path))
        try:
            fp = open(config_file_path)
            content = fp.read()
        except OSError as reason:
            log_error("load the configuration failed, error: %s" % str(reason))
        request_data = {"level": 1, "application": self.app_name,
                        "server_name": "", "filename": config_file_name, "config": content}
        response = requests.post(
            self.add_config_url, params=self.token_param, json=request_data)
        if TarsTool.parse_response("add application config file", response) is True:
            return True
        if response.status_code != 200:
            return False
        # try to update config
        log_info("add config file failed, try to update the config")
        return self.update_app_config(config_file_name, config_file_path)

    def update_app_config(self, config_file_name, config_file_path):
        log_info("update config file for application %s, config file path: %s" %
                 (self.app_name, config_file_path))
        ret, config_id = self.get_config_file_id(config_file_name)
        if ret is False:
            return False
        try:
            fp = open(config_file_path)
            content = fp.read()
        except OSError as reason:
            log_error("load the configuration failed, error: %s" % str(reason))
        request_data = {"id": config_id, "config": content,
                        "reason": "update config file"}
        response = requests.post(
            self.update_config_url, params=self.token_param, json=request_data)
        if TarsTool.parse_response("update config file for application " + self.app_name + ", config file:" + config_file_name, response) is False:
            return False
        return True

    def get_config_file_id(self, config_file_name):
        log_info("query the config file id for %s" % config_file_name)
        params = {"ticket": self.tars_token, "level": 1, "application": self.app_name,
                  "server_name": "", "set_name": "", "set_area": "", "set_group": ""}
        response = requests.get(
            self.config_file_list_url, params=params)
        if TarsTool.parse_response("query the config file id for " + config_file_name, response) is False:
            return (False, 0)
        result = response.json()
        if "data" not in result or len(result["data"]) == 0:
            log_error(
                "query the config file id failed for %s because of empty return data" % config_file_name)
            return (False, 0)
        # try to find the config file info
        for item in result["data"]:
            if "filename" in item and item["filename"] == config_file_name:
                return (True, item["id"])
        log_error("the config file %s not found" % config_file_name)
        return (False, 0)

    def add_app_config_list(self, config_list, config_file_list):
        i = 0
        for config_file_path in config_file_list:
            config = config_list[i]
            if self.add_app_config_file(config, config_file_path) is False:
                log_error("add_app_config_list failed, config files info: %s" %
                          config_list)
            i = i+1
        return True

    def upload_tars_package(self, service_name, package_path):
        """
        upload the tars package
        """
        log_info("upload tars package for service %s, packge_path: %s" %
                 (service_name, package_path))
        if os.path.exists(package_path) is False:
            log_error("upload tars package for service %s failed for the path %s not exists" % (
                service_name, package_path))
            return (False, 0)
        package_name = service_name + tars_pkg_post_fix
        form_data = MultipartEncoder(fields={"application": self.app_name, "module_name": service_name, "comment": "upload package", "suse": (
            package_name, open(package_path, 'rb'), 'text/plain/binary')})
        response = requests.post(self.upload_package_url, data=form_data, params=self.token_param, headers={
                                 'Content-Type': form_data.content_type})
        if TarsTool.parse_response("upload tars package " + package_path, response) is False:
            return (False, 0)
        # get the id
        result = response.json()
        if 'data' not in result:
            log_error("upload tar package %s failed for empty return message: %s" %
                      (package_path, result))
            return (False, 0)
        result_data = result['data']
        if 'id' not in result_data:
            log_error("upload tar package %s failed for empty return message: %s" %
                      (package_path, result))
            return (False, 0)
        return (True, result_data['id'])

    def get_server_info(self, tree_node_id):
        params = {'tree_node_id': tree_node_id, "ticket": self.tars_token}
        response = requests.get(self.get_server_list_url, params=params)
        if TarsTool.parse_response("get server info by tree node id: " + tree_node_id, response) is False:
            log_error("get server info by tree node id for error response, tree_node_id: %s, msg:" % (
                tree_node_id, response.content))
            return (False, response)
        return (True, response)

    def app_exists(self):
        (ret, response) = self.get_server_info("1" + self.app_name)
        if ret is False:
            return False
        result = response.json()
        if 'data' in result and len(result["data"]) > 0:
            return True
        return False

    def server_exists(self, service_name):
        node_tree_id = "1" + self.app_name + ".5" + service_name
        (ret, response) = self.get_server_info(node_tree_id)
        if ret is False:
            return False
        result = response.json()
        if 'data' in result and len(result["data"]) > 0:
            return True
        return False

    def get_server_id(self, service_name):
        # tree_node_id
        tree_node_id = "1" + self.app_name + ".5" + service_name
        (ret, response) = self.get_server_info(tree_node_id)
        if ret is False:
            return (False, 0)
        if TarsTool.parse_response("get server list ", response) is False:
            log_error("get server info failed for error response, server name: %s, msg:" % (
                service_name, response.content))
            return (False, 0)
        result = response.json()
        if "data" not in result or len(result["data"]) == 0 or "id" not in result["data"][0]:
            log_error("get server info failed for empty return, server name: %s" %
                      service_name)
            return (False, 0)
        server_id = result["data"][0]["id"]
        return (True, server_id)

    def upload_and_publish_package(self, service_name, package_path):
        """
        upload and publish the tars package
        """
        # get the service info
        (ret, server_id) = self.get_server_id(service_name)
        if ret is False:
            log_error(
                "upload and publish package failed for get the server info failed, server: %s" % service_name)
            return False
        # upload the tars package
        (ret, patch_id) = self.upload_tars_package(service_name, package_path)
        if ret is False:
            return False
        # patch tars
        self.patch_tars(server_id, patch_id)
        return True

    def patch_tars(self, server_id, patch_id):
        log_info("patch tars for application %s" % self.app_name)
        items = [{"server_id": server_id, "command": "patch_tars", "parameters": {
            "patch_id": patch_id, "bak_flag": 'false', "update_text": "", "group_name": ""}}]
        request_data = {"serial": 'true', "items": items}
        response = requests.post(
            self.add_task_url, params=self.token_param, json=request_data)
        if TarsTool.parse_response("patch tars ", response) is False:
            log_error("patch tars failed for error response, server name: %s, msg: %s" % (
                service_name, response.content))
            return False
        return True

    def add_task(self, service_name, command):
        """
        current supported commands are: stop, restart, undeploy_tars, patch_tars
        """
        log_info("add_task for service %s, command is %s" %
                 (service_name, command))
        (ret, server_id) = self.get_server_id(service_name)
        if ret is False:
            return False
        items = [{"server_id": server_id, "command": command, "parameters": {}}]
        request_data = {"serial": 'true', "items": items}
        response = requests.post(
            self.add_task_url, params=self.token_param, json=request_data)
        if TarsTool.parse_response("execute command " + command, response) is False:
            log_error("add_task failed for error response, server name: %s, msg: %s" % (
                service_name, response.content))
            return False
        return True

    def stop_server(self, service_name):
        """
        stop the givn service
        """
        return self.add_task(service_name, "stop")

    def stop_server_list(self, server_list):
        for server in server_list:
            if self.stop_server(server) is False:
                return False
        return True

    def restart_server(self, service_name):
        """
        restart the given service
        """
        return self.add_task(service_name, "restart")

    def undeploy_tars(self, service_name):
        """
        undeploy the tars service
        """
        return self.add_task(service_name, "undeploy_tars")

    def undeploy_server_list(self, server_list):
        for server in server_list:
            if self.undeploy_tars(server) is False:
                return False
        return True

    def restart_server_list(self, server_list):
        for server in server_list:
            if self.restart_server(server) is False:
                return False
            time.sleep(5)
        return True

    def get_service_list(self):
        return self.get_server_info("1" + self.app_name)

    def upload_and_publish_package_list(self, service_list, service_path_list):
        i = 0
        for service in service_list:
            service_path = service_path_list[i]
            self.upload_and_publish_package(service, service_path)
            i = i+1
            time.sleep(10)


def generate_pkg_path_list(config):
    service_path_list = []
    for service in service_list:
        service_path_list.append(
            config.tars_pkg_dir + "/" + service + tars_pkg_post_fix)
    return service_path_list


def generate_config_file_list(config_base_dir):
    config_info = []
    for config_file in config_list:
        config_info.append(config_base_dir + "/" + config_file)
    return config_info


def generate_block_chain_config(config):
    "generate the blockchain config"
    sm_mode = ""
    if config.sm_mode is True:
        sm_mode = " -s"
    ip_params = ""
    for deploy_info in config.deploy_info_list:
        ip_params += deploy_info.deploy_ip + \
            ":" + str(deploy_info.node_count) + ","
    ip_params = ip_params[0:-1]
    generate_config_cmd = "bash generate_config.sh -l %s -o %s -p %d,%d -r %d -c %d -g %s -b %d -n pbft %s" % (
        ip_params, config.app_name, config.p2p_listen_port, config.rpc_listen_port, config.leader_period, config.consensus_timeout, config.gas_limit, config.block_tx_count_limit, sm_mode)

    # generate the node config
    status, output = subprocess.getstatusoutput(generate_config_cmd)
    # generate config failed
    if status != 0:
        log_error(
            "generate the blockchain config failed, error message: %s" % output)
        return False
    # generate config success
    log_info("generate the blockchain config success")
    return True


def generate_app_service_list(config):
    app_list = []
    config_base_dir_info = {}
    i = 0
    j = 0
    for item in config.deploy_info_list:
        for i in range(item.node_count):
            app_name = config.app_name + "Ip" + str(j) + "Id" + str(i)
            config_base_dir_info[app_name] = config.app_name + \
                "/" + item.deploy_ip + "/node" + str(i)
            tars_tool = TarsTool(
                config.tars_url, config.tars_token, app_name, item.deploy_ip)
            app_list.append(tars_tool)
            i = i + 1
        j = j + 1
    return (True, app_list, config_base_dir_info)


def generate_tars_servers(config):
    # create the app
    ret, app_list, config_base_dir_info = generate_app_service_list(config)
    if ret is False:
        return False
    # create service
    for app_service in app_list:
        # create the application
        log_info("begin create application %s" % app_service.app_name)
        ret = app_service.create_application()
        if ret is False:
            return (False)
        # create the service
        ret = app_service.deploy_service_list(service_list)
        if ret is False:
            return False
        # add configuration file
        config_info = generate_config_file_list(
            config_base_dir_info[app_service.app_name])
        ret = app_service.add_app_config_list(config_list, config_info)
        if ret is False:
            return False
        service_path_list = generate_pkg_path_list(config)
        ret = app_service.upload_and_publish_package_list(
            service_list, service_path_list)
        if ret is False:
            return False
    return True


def restart_all(config):
    ret, app_list, config_base_dir_info = generate_app_service_list(config)
    if ret is False:
        return False
    for app_service in app_list:
        app_service.restart_server_list(service_list)
        time.sleep(5)
    return True


def upload_all_tars_package(config):
    ret, app_list, config_base_dir_info = generate_app_service_list(config)
    service_path_list = generate_pkg_path_list(config)
    for app in app_list:
        if app.upload_and_publish_package_list(service_list, service_path_list) is False:
            return False
    return True


def stop_all(config):
    ret, app_list, config_base_dir_info = generate_app_service_list(config)
    if ret is False:
        return False
    for app_service in app_list:
        app_service.stop_server_list(service_list)
    return True


def undeploy_all(config):
    ret, app_list, config_base_dir_info = generate_app_service_list(config)
    if ret is False:
        return False
    for app_service in app_list:
        app_service.undeploy_server_list(service_list)
    return True


def create_blockchain_nodes(config):
    ret = generate_block_chain_config(config)
    if ret is False:
        return False
    ret = generate_tars_servers(config)
    if ret is False:
        return False
    return True


def parse_config_file(toml_file_path):
    config = toml.load(toml_file_path)
    return BuildChainConfig(config)


def parse_command():
    parser = argparse.ArgumentParser(description='build_chain')
    parser.add_argument(
        '--command', help="[Required]the command, current only support: generate_config/create_service/build_chain/restart_all/stop_all/undeploy_all/upload_all")
    args = parser.parse_args()
    return args


def main():
    config = parse_config_file("config.toml")
    args = parse_command()
    if args.command is None or args.command == "":
        log_error("Must set command, current supported_commands are: generate_config/create_service/build_chain/restart_all/stop_all/undeploy_all/upload_all")
        return
    if args.command == "generate_config":
        ret = generate_block_chain_config(config)
        if ret is True:
            log_info("create the blockchain config success")
        else:
            log_error("create the blockchain config failed")
        return
    if args.command == "create_service":
        ret = generate_tars_servers(config)
        if ret is True:
            log_info("create the blockchain service success")
        else:
            log_error(
                "create the blockchain service failed")
        return
    if args.command == "build_chain":
        ret = create_blockchain_nodes(config)
        if ret is True:
            log_info("create the blockchain and upload the service success")
        else:
            log_error("create the blockchain failed")
        return
    if args.command == "restart_all":
        ret = restart_all(config)
        if ret is True:
            log_info("restart the blockchain and upload the service success")
        else:
            log_error("restart the blockchain failed")
        return
    if args.command == "stop_all":
        ret = stop_all(config)
        if ret is True:
            log_info("stop the blockchain and upload the service success")
        else:
            log_error("stop the blockchain failed")
        return
    if args.command == "upload_all":
        ret = upload_all_tars_package(config)
        if ret is True:
            log_info("Upload the tars package success")
        else:
            log_error("Upload the tars package failed")
        return
    if args.command == "undeploy_all":
        ret = undeploy_all(config)
        if ret is True:
            log_info("undeploy the blockchain and upload the service success")
        else:
            log_error("undeploy the blockchain failed")
        return
    log_error("Unsupported command %s, current supported commands are generate_config/create_service/build_chain/restart_all/stop_all/undeploy_all/upload_all" % args.command)


if __name__ == "__main__":
    main()
