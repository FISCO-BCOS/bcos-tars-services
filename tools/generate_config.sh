#!/bin/bash

dirpath="$(cd "$(dirname "$0")" && pwd)"
listen_ip="0.0.0.0"
port_start=(30300 20200)
p2p_listen_port=port_start[0]
rpc_listen_port=port_start[1]
use_ip_param=
ip_array=
output_dir=""
cdn_link_header="https://osp-1257653870.cos.ap-guangzhou.myqcloud.com/FISCO-BCOS"

# for cert generation
ca_cert_dir="${dirpath}"
sm_cert_conf='sm_cert.cnf'
days=36500
rsa_key_length=2048
sm_mode='false'
macOS=""
x86_64_arch="true"
sm2_params="sm_sm2.param"
OPENSSL_CMD="${HOME}/.fisco/tassl"
nodeid_list=""
nodes_json_file_name="nodes.json"
leader_period=1
block_tx_count_limit=1000
consensus_timeout=3000
consensus_type="pbft"
gas_limit="300000000"
file_dir="./"

LOG_WARN() {
    local content=${1}
    echo -e "\033[31m[ERROR] ${content}\033[0m"
}

LOG_INFO() {
    local content=${1}
    echo -e "\033[32m[INFO] ${content}\033[0m"
}

LOG_FALT() {
    local content=${1}
    echo -e "\033[31m[FALT] ${content}\033[0m"
    exit 1
}

dir_must_exists() {
    if [ ! -d "$1" ]; then
        LOG_FALT "$1 DIR does not exist, please check!"
    fi
}

file_must_not_exists() {
    if [ -f "$1" ]; then
        LOG_FALT "$1 file already exist, please check!"
    fi
}

file_must_exists() {
    if [ ! -f "$1" ]; then
        LOG_FALT "$1 file does not exist, please check!"
    fi
}

check_env() {
    if [ "$(uname)" == "Darwin" ];then
        export PATH="/usr/local/opt/openssl/bin:$PATH"
        macOS="macOS"
    fi
    [ ! -z "$(openssl version | grep 1.0.2)" ] || [ ! -z "$(openssl version | grep 1.1)" ] || {
        echo "please install openssl!"
        #echo "download openssl from https://www.openssl.org."
        echo "use \"openssl version\" command to check."
        exit 1
    }

    if [ "$(uname -m)" != "x86_64" ];then
        x86_64_arch="false"
    fi
}

check_name() {
    local name="$1"
    local value="$2"
    [[ "$value" =~ ^[a-zA-Z0-9._-]+$ ]] || {
        LOG_FALT "$name name [$value] invalid, it should match regex: ^[a-zA-Z0-9._-]+\$"
    }
}

generate_sm_sm2_param() {
    local output=$1
    cat <<EOF >"${output}"
-----BEGIN EC PARAMETERS-----
BggqgRzPVQGCLQ==
-----END EC PARAMETERS-----

EOF
}

generate_sm_cert_conf() {
    local output=$1
    cat <<EOF >"${output}"
HOME			= .
RANDFILE		= $ENV::HOME/.rnd
oid_section		= new_oids

[ new_oids ]
tsa_policy1 = 1.2.3.4.1
tsa_policy2 = 1.2.3.4.5.6
tsa_policy3 = 1.2.3.4.5.7

####################################################################
[ ca ]
default_ca	= CA_default		# The default ca section

####################################################################
[ CA_default ]

dir		= ./demoCA		# Where everything is kept
certs		= $dir/certs		# Where the issued certs are kept
crl_dir		= $dir/crl		# Where the issued crl are kept
database	= $dir/index.txt	# database index file.
#unique_subject	= no			# Set to 'no' to allow creation of
					# several ctificates with same subject.
new_certs_dir	= $dir/newcerts		# default place for new certs.

certificate	= $dir/cacert.pem 	# The CA certificate
serial		= $dir/serial 		# The current serial number
crlnumber	= $dir/crlnumber	# the current crl number
					# must be commented out to leave a V1 CRL
crl		= $dir/crl.pem 		# The current CRL
private_key	= $dir/private/cakey.pem # The private key
RANDFILE	= $dir/private/.rand	# private random number file

x509_extensions	= usr_cert		# The extensions to add to the cert

name_opt 	= ca_default		# Subject Name options
cert_opt 	= ca_default		# Certificate field options

default_days	= 365			# how long to certify for
default_crl_days= 30			# how long before next CRL
default_md	= default		# use public key default MD
preserve	= no			# keep passed DN ordering

policy		= policy_match

[ policy_match ]
countryName		= match
stateOrProvinceName	= match
organizationName	= match
organizationalUnitName	= optional
commonName		= supplied
emailAddress		= optional

[ policy_anything ]
countryName		= optional
stateOrProvinceName	= optional
localityName		= optional
organizationName	= optional
organizationalUnitName	= optional
commonName		= supplied
emailAddress		= optional

####################################################################
[ req ]
default_bits		= 2048
default_md		= sm3
default_keyfile 	= privkey.pem
distinguished_name	= req_distinguished_name
x509_extensions	= v3_ca	# The extensions to add to the self signed cert

string_mask = utf8only

# req_extensions = v3_req # The extensions to add to a certificate request

[ req_distinguished_name ]
countryName = CN
countryName_default = CN
stateOrProvinceName = State or Province Name (full name)
stateOrProvinceName_default =GuangDong
localityName = Locality Name (eg, city)
localityName_default = ShenZhen
organizationalUnitName = Organizational Unit Name (eg, section)
organizationalUnitName_default = fisco
commonName =  Organizational  commonName (eg, fisco)
commonName_default =  fisco
commonName_max = 64

[ usr_cert ]
basicConstraints=CA:FALSE
nsComment			= "OpenSSL Generated Certificate"

subjectKeyIdentifier=hash
authorityKeyIdentifier=keyid,issuer

[ v3_req ]

# Extensions to add to a certificate request

basicConstraints = CA:FALSE
keyUsage = nonRepudiation, digitalSignature

[ v3enc_req ]

# Extensions to add to a certificate request
basicConstraints = CA:FALSE
keyUsage = keyAgreement, keyEncipherment, dataEncipherment

[ v3_agency_root ]
subjectKeyIdentifier=hash
authorityKeyIdentifier=keyid:always,issuer
basicConstraints = CA:true
keyUsage = cRLSign, keyCertSign

[ v3_ca ]
subjectKeyIdentifier=hash
authorityKeyIdentifier=keyid:always,issuer
basicConstraints = CA:true
keyUsage = cRLSign, keyCertSign

EOF
}

generate_cert_conf() {
    local output=$1
    cat <<EOF >"${output}"
[ca]
default_ca=default_ca
[default_ca]
default_days = 3650
default_md = sha256

[req]
distinguished_name = req_distinguished_name
req_extensions = v3_req
[req_distinguished_name]
countryName = CN
countryName_default = CN
stateOrProvinceName = State or Province Name (full name)
stateOrProvinceName_default =GuangDong
localityName = Locality Name (eg, city)
localityName_default = ShenZhen
organizationalUnitName = Organizational Unit Name (eg, section)
organizationalUnitName_default = FISCO-BCOS
commonName =  Organizational  commonName (eg, FISCO-BCOS)
commonName_default = FISCO-BCOS
commonName_max = 64

[ v3_req ]
basicConstraints = CA:FALSE
keyUsage = nonRepudiation, digitalSignature, keyEncipherment

[ v4_req ]
basicConstraints = CA:TRUE

EOF
}

gen_chain_cert() {

    if [ ! -f "${cert_conf}" ]; then
        generate_cert_conf "${cert_conf}"
    fi

    local chaindir="${1}"

    file_must_not_exists "${chaindir}"/ca.key
    file_must_not_exists "${chaindir}"/ca.crt
    file_must_exists "${cert_conf}"

    mkdir -p "$chaindir"
    dir_must_exists "$chaindir"

    openssl genrsa -out "${chaindir}"/ca.key "${rsa_key_length}" 2> /dev/null
    openssl req -new -x509 -days "${days}" -subj "/CN=FISCO-BCOS/O=FISCO-BCOS/OU=chain" -key "${chaindir}"/ca.key -out "${chaindir}"/ca.crt 2>/dev/null
    mv "${cert_conf}" "${chaindir}"

    LOG_INFO "Build ca cert successfully!"
}

gen_rsa_node_cert() {
    local capath="${1}"
    local ndpath="${2}"
    local node="${3}"

    file_must_exists "$capath/ca.key"
    file_must_exists "$capath/ca.crt"
    check_name node "$node"

    file_must_not_exists "$ndpath"/ssl.key
    file_must_not_exists "$ndpath"/ssl.crt

    mkdir -p "${ndpath}"
    dir_must_exists "${ndpath}"

    openssl genrsa -out "${ndpath}"/ssl.key "${rsa_key_length}" 2> /dev/null
    openssl req -new -sha256 -subj "/CN=FISCO-BCOS/O=fisco-bcos/OU=agency" -key "$ndpath"/ssl.key -config "$capath"/cert.cnf -out "$ndpath"/ssl.csr
    openssl x509 -req -days "${days}" -sha256 -CA "${capath}"/ca.crt -CAkey "$capath"/ca.key -CAcreateserial \
        -in "$ndpath"/ssl.csr -out "$ndpath"/ssl.crt -extensions v4_req -extfile "$capath"/cert.cnf 2>/dev/null

    openssl pkcs8 -topk8 -in "$ndpath"/ssl.key -out "$ndpath"/pkcs8_node.key -nocrypt
    cp "$capath"/ca.crt "$capath"/cert.cnf "$ndpath"/

    rm -f "$ndpath"/ssl.csr
    rm -f "$ndpath"/ssl.key

    mv "$ndpath"/pkcs8_node.key "$ndpath"/ssl.key

    LOG_INFO "Build ${node} cert successful!"
}

gen_sm_chain_cert() {
    local chaindir="${1}"
    name=$(basename "$chaindir")
    check_name chain "$name"

    if [ ! -f "${sm_cert_conf}" ]; then
        generate_sm_cert_conf 'sm_cert.cnf'
    else
        cp -f "${sm_cert_conf}" .
    fi

    generate_sm_sm2_param "${sm2_params}"

    mkdir -p "$chaindir"
    dir_must_exists "$chaindir"

    "$OPENSSL_CMD" genpkey -paramfile "${sm2_params}" -out "$chaindir/sm_ca.key"
    "$OPENSSL_CMD" req -config sm_cert.cnf -x509 -days "${days}" -subj "/CN=FISCO-BCOS/O=FISCO-BCOS/OU=chain" -key "$chaindir/sm_ca.key" -extensions v3_ca -out "$chaindir/sm_ca.crt" 2>/dev/null
    cp "${sm_cert_conf}" "${chaindir}"
    cp "${sm2_params}" "${chaindir}"
}

gen_sm_node_cert_with_ext() {
    local capath="$1"
    local certpath="$2"
    local name="$3"
    local type="$4"
    local extensions="$5"

    file_must_exists "$capath/sm_ca.key"
    file_must_exists "$capath/sm_ca.crt"

    file_must_not_exists "$ndpath/sm_${type}.crt"
    file_must_not_exists "$ndpath/sm_${type}.key"

    "$OPENSSL_CMD" genpkey -paramfile "$capath/${sm2_params}" -out "$certpath/sm_${type}.key"
    "$OPENSSL_CMD" req -new -subj "/CN=$name/O=fisco-bcos/OU=${type}" -key "$certpath/sm_${type}.key" -config "$capath/sm_cert.cnf" -out "$certpath/sm_${type}.csr"

    # echo "not use $(basename "$capath") to sign $(basename $certpath) ${type}" >>"${logfile}"
    "$OPENSSL_CMD" x509 -sm3 -req -CA "$capath/sm_ca.crt" -CAkey "$capath/sm_ca.key" -days "${days}" -CAcreateserial -in "$certpath/sm_${type}.csr" -out "$certpath/sm_${type}.crt" -extfile "$capath/sm_cert.cnf" -extensions "$extensions"

    rm -f "$certpath/sm_${type}.csr"
}

gen_sm_node_cert() {
    local capath="${1}"
    local ndpath="${2}"

    file_must_exists "$capath/sm_ca.key"
    file_must_exists "$capath/sm_ca.crt"

    mkdir -p "$ndpath"
    dir_must_exists "$ndpath"
    local node=$(basename "$ndpath")
    check_name node "$node"

    gen_sm_node_cert_with_ext "$capath" "$ndpath" "$node" ssl v3_req
    gen_sm_node_cert_with_ext "$capath" "$ndpath" "$node" enssl v3enc_req
    #nodeid is pubkey
    $OPENSSL_CMD ec -in "$ndpath/sm_ssl.key" -text -noout 2> /dev/null | sed -n '7,11p' | sed 's/://g' | tr "\n" " " | sed 's/ //g' | awk '{print substr($0,3);}' | cat > "$ndpath/sm_ssl.nodeid"

    cp "$capath/sm_ca.crt" "$ndpath"
}

help() {
    echo $1
    cat <<EOF
Usage:
    -l <IP list>                        [Required] "ip1:nodeNum1,ip2:nodeNum2" e.g:"192.168.0.1:2,192.168.0.2:3"
    -o <output dir>                     [Optional] output directory, default ./nodes
    -p <start Port>                     [Optional] Default 30300,20200 means p2p port start from 30300, rpc port start from 20200
    -s <SM model>                       [Optional] SM SSL connection or not, default no
    -r <leader_period>                  [Optional] leader_period, default 1
    -b <block_tx_count_limit>           [Optional] block_tx_count_limit, default 1000
    -c <consensus_timeout>              [Optional] consensus_timeout, default 3000
    -g <gas_limit>                      [Optional] gas_limit default 300000000
    -n <consensus_type>                 [Optional] consensus_type default pbft
    -h Help
e.g
    bash $0 -p 30300,20200 -l 127.0.0.1:4 -o nodes -e ./mini-consensus
    bash $0 -p 30300,20200 -l 127.0.0.1:4 -o nodes -e ./mini-consensus -s
EOF

    exit 0
}

parse_params() {
    while getopts "l:r:b:c:g:n:o:p:sh" option; do
        case $option in
        l) ip_param=$OPTARG
            use_ip_param="true"
            ;;
        r) leader_period="$OPTARG"
        ;;
        b) block_tx_count_limit="$OPTARG"
        ;;
        c) consensus_timeout="$OPTARG"
        ;;
        g) gas_limit="$OPTARG"
        ;;
        n) consensus_type="$OPTARG"
        ;;
        o)
            output_dir="$OPTARG"
            mkdir -p "$output_dir"
            dir_must_exists "${output_dir}"
            ;;
        p) port_start=(${OPTARG//,/ })
        if [ ${#port_start[@]} -ne 2 ];then LOG_WARN "p2p start port error. e.g: 30300" && exit 1;fi
            p2p_listen_port=port_start[0]
            rpc_listen_port=port_start[1]
        ;;
        s) sm_mode="true" ;;
        h) help ;;
        *) help ;;
        esac
    done
}
print_result() {
    echo "=============================================================="
    LOG_INFO "Start Port        : ${port_start[*]}"
    LOG_INFO "Server IP         : ${ip_array[*]}"
    LOG_INFO "SSL Model         : ${ssl_model}"
    LOG_INFO "output dir         : ${output_dir}"
    LOG_INFO "All completed. Files in ${output_dir}"
}
generate_node_cert() {
    local sm_mode="$1"
    local ca_cert_path="${2}"
    local node_cert_path="${3}"
    mkdir -p ${node_cert_path}
    if [[ "${sm_mode}" == "false" ]]; then
        gen_rsa_node_cert "${ca_cert_path}" "${node_cert_path}" "node" 2>&1
    else
        gen_sm_node_cert "${ca_cert_path}" "${node_cert_path}" "node" 2>&1
    fi
}

generate_chain_cert(){
    local sm_mode="$1"
    local chain_cert_path="$2"
    mkdir -p "${chain_cert_path}"
    if [[ "${sm_mode}" == "false" ]]; then
        gen_chain_cert "${chain_cert_path}" 2>&1
    else
        gen_sm_chain_cert "${chain_cert_path}" 2>&1
    fi
}
generate_config_ini() {
    local output="${1}"
    local p2p_listen_port="${2}"
    local rpc_listen_port="${3}"
    cat <<EOF >"${output}"
[p2p]
    listen_ip=${listen_ip}
    listen_port=${p2p_listen_port}
    ; ssl or sm ssl
    sm_ssl=false
    nodes_path=${file_dir}
    nodes_file=${nodes_json_file_name}

[rpc]
    listen_ip=${listen_ip}
    listen_port=${rpc_listen_port}
    thread_count=4

[cert]
    ; directory the certificates located in
    ca_path=./
    ; the ca certificate file
    ca_cert=ca.crt
    ; the node private key file
    node_key=ssl.key
    ; the node certificate file
    node_cert=ssl.crt
EOF
    generate_common_ini "${output}"
}

generate_common_ini() {
    local output=${1}

    cat <<EOF >>"${output}"

[chain]
    ; use SM crypto or not, should nerver be changed
    sm_crypto=${sm_mode}
    ; the group id, should nerver be changed
    group_id=group
    ; the chain id, should nerver be changed
    chain_id=chain
    ; the block limit, should nerver be changed
    block_limit=1000

[security]
    private_key_path=node.pem
    checkpoint_timeout=3000

[consensus]
    ; min block generation time(ms)
    min_seal_time=500

[executor]
    ; use the wasm virtual machine or not
    is_wasm=false
    is_auth_check=false

[storage]
    data_path=data

[txpool]
    limit=15000
    notify_worker_num=2
    verify_worker_num=2
[log]
    enable=true
    log_path=./log
    ; network statistics interval, unit is second, default is 60s
    stat_flush_interval=60
    ; info debug trace
    level=DEBUG
    ; MB
    max_log_file_size=200
EOF
}

generate_sm_config_ini() {
    local output=${1}
    local p2p_listen_port="${2}"
    local rpc_listen_port="${3}"
    cat <<EOF >"${output}"
[p2p]
    listen_ip=${listen_ip}
    listen_port=${p2p_listen_port}
    ; ssl or sm ssl
    sm_ssl=true
    nodes_path=${file_dir}
    nodes_file=${nodes_json_file_name}

[rpc]
    listen_ip=${listen_ip}
    listen_port=${rpc_listen_port}
    thread_count=4

[cert]
    ; directory the certificates located in
    ca_path=./
    ; the ca certificate file
    sm_ca_cert=sm_ca.crt
    ; the node private key file
    sm_node_key=sm_ssl.key
    ; the node certificate file
    sm_node_cert=sm_ssl.crt
    ; the node private key file
    sm_ennode_key=sm_enssl.key
    ; the node certificate file
    sm_ennode_cert=sm_enssl.crt
EOF
    generate_common_ini "${output}"
}

generate_nodes_json() {
    local output=${1}
    local p2p_host_list=""
    local ip_params="${2}"
    local ip_array=(${ip_params//,/ })
    local ip_length=${#ip_array[@]}
    local i=0
    for ((; i < ip_length; i++)); do
        local ip=${ip_array[i]}
        local delim=""
        if [[ $i == $((ip_length - 1)) ]]; then
            delim=""
        else
            delim=","
        fi
        p2p_host_list="${p2p_host_list}\"${ip}\"${delim}"
    done

    cat <<EOF >"${output}"
{"nodes":[${p2p_host_list}]}
EOF
}

generate_config()
{
    local sm_mode="${1}"
    local node_config_path="${2}"
    local node_json_config_path="${3}"
    local connected_nodes="${4}"
    local p2p_listen_port="${5}"
    local rpc_listen_port="${6}"
    if [ "${sm_mode}" == "false" ]; then
        generate_config_ini "${node_config_path}" "${p2p_listen_port}" "${rpc_listen_port}"
    else
        generate_sm_config_ini "${node_config_path}" "${p2p_listen_port}" "${rpc_listen_port}"
    fi
    generate_nodes_json "${node_json_config_path}/${nodes_json_file_name}" "${connected_nodes}"
}

generate_node_account()
{
    local output_path="${1}"
    local node_index="${2}"
    if [ ! -d "${output_path}" ];then
        mkdir -p ${output_path}
    fi
    if [ ! -f /tmp/secp256k1.param ];then
        ${OPENSSL_CMD} ecparam -out /tmp/secp256k1.param -name secp256k1
    fi
    ${OPENSSL_CMD} genpkey -paramfile /tmp/secp256k1.param -out ${output_path}/node.pem
    # generate nodeid
    ${OPENSSL_CMD} ec -text -noout -in "${output_path}/node.pem" 2> /dev/null | sed -n '7,11p' | tr -d ": \n" | awk '{print substr($0,3);}' | cat >"$output_path"/node.nodeid
    local node_id=$(cat "${output_path}/node.nodeid")
    nodeid_list=$"${nodeid_list}node.${node_index}=${node_id}: 1
    "
}

generate_sm_node_account()
{
    local output_path="${1}"
    local node_index="${2}"
    if [ ! -d "${output_path}" ];then
        mkdir -p ${output_path}
    fi
    if [ ! -f ${sm2_params} ];then
        generate_sm_sm2_param ${sm2_params}
    fi
    ${OPENSSL_CMD} genpkey -paramfile ${sm2_params} -out ${output_path}/node.pem 2>/dev/null
    $OPENSSL_CMD ec -in "$output_path/node.pem" -text -noout 2> /dev/null | sed -n '7,11p' | sed 's/://g' | tr "\n" " " | sed 's/ //g' | awk '{print substr($0,3);}'  | cat > "$output_path/node.nodeid"
    local node_id=$(cat "${output_path}/node.nodeid")
    nodeid_list=$"${nodeid_list}node.${node_index}=${node_id}:1
    "
}

generate_genesis_config()
{
    local output=${1}
    local node_list=${2}

    cat <<EOF >"${output}"
[consensus]
    ; consensus algorithm now support PBFT(consensus_type=pbft)
    consensus_type=${consensus_type}
    ; the max number of transactions of a block
    block_tx_count_limit=${block_tx_count_limit}
    ; in millisecond, block consensus timeout, at least 3000ms
    consensus_timeout=${consensus_timeout}
    ; the number of blocks generated by each leader
    leader_period=${leader_period}
    ; the node id of consensusers
    ${node_list}
[tx]
    ; transaction gas limit
    gas_limit=${gas_limit}
EOF
}

parse_ip_config()
{
    local config=$1
    n=0
    while read line;do
        ip_array[n]=$(echo ${line} | awk '{print $1}')
        agency_array[n]=$(echo ${line} | awk '{print $2}')
        group_array[n]=$(echo ${line} | awk '{print $3}')
        if [ -z "${ip_array[$n]}" -o -z "${agency_array[$n]}" -o -z "${group_array[$n]}" ];then
            exit_with_clean "Please check ${config}, make sure there is no empty line!"
        fi
        ((++n))
    done < ${config}
}

get_value()
{
    local var_name=${1}
    var_name=var_${var_name//./}
    local res=$(eval echo '$'"${var_name}")
    echo ${res}
}


set_value()
{
    local var_name=${1}
    var_name=var_${var_name//./}
    local var_value=${2}
    eval "${var_name}=${var_value}"
}

exit_with_clean()
{
    local content=${1}
    echo -e "\033[31m[ERROR] ${content}\033[0m"
    if [ -d "${output_dir}" ];then
        rm -rf ${output_dir}
    fi
    exit 1
}

check_and_install_tassl(){
if [ -n "${sm_mode}" ]; then
    if [ ! -f "${OPENSSL_CMD}" ];then
        local tassl_link_perfix="${cdn_link_header}/FISCO-BCOS/tools/tassl-1.0.2"
        LOG_INFO "Downloading tassl binary from ${tassl_link_perfix}..."
        if [[ -n "${macOS}" ]];then
            curl -#LO "${tassl_link_perfix}/tassl_mac.tar.gz"
            mv tassl_mac.tar.gz tassl.tar.gz
        else
            if [[ "$(uname -p)" == "aarch64" ]];then
                curl -#LO "${tassl_link_perfix}/tassl-aarch64.tar.gz"
                mv tassl-aarch64.tar.gz tassl.tar.gz
            elif [[ "$(uname -p)" == "x86_64" ]];then
                curl -#LO "${tassl_link_perfix}/tassl.tar.gz"
            else
                LOG_ERROR "Unsupported platform"
                exit 1
            fi
        fi
        tar zxvf tassl.tar.gz && rm tassl.tar.gz
        chmod u+x tassl
        mkdir -p "${HOME}"/.fisco
        mv tassl "${HOME}"/.fisco/tassl
    fi
fi
}

main() {
    # FIXME: use openssl 1.1 to generate gm certificates
    check_env
    check_and_install_tassl
    parse_params "$@"
    cert_conf="${output_dir}/cert.cnf"

    [ -z $use_ip_param ] && help 'ERROR: Please set -l or -f option.'
    if [ "${use_ip_param}" == "true" ];then
        ip_array=(${ip_param//,/ })
    elif [ "${use_ip_param}" == "false" ];then
        if ! parse_ip_config $ip_file ;then 
            exit_with_clean "Parse $ip_file error!"
        fi
    else 
        help 
    fi
    local i=0
    node_count=0
    local count=0
    connected_nodes=""
    ca_dir="${output_dir}/ca"
    generate_chain_cert "${sm_mode}" "${ca_dir}"
    # Note: must generate the node account firstly
    for line in ${ip_array[*]};do
        ip=${line%:*}
        num=${line#*:}
        if [ -z $(echo $ip | grep -E "^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$") ];then
            LOG_WARN "Please check IP address: ${ip}, if you use domain name please ignore this."
        fi
        [ "$num" == "$ip" ] || [ -z "${num}" ] && num=${node_num}
        echo "Processing IP:${ip} Total:${num}" 
        [ -z "$(get_value ${ip//./}_count)" ] && set_value ${ip//./}_count 0
        
        nodes_dir="${output_dir}/${ip}"
        ca_cert_dir="${nodes_dir}/ca"
        mkdir -p "${ca_cert_dir}"
        cp -r ${ca_dir}/* ${ca_cert_dir}
        for ((i=0;i<num;++i));do
            local node_count=$(get_value ${ip//./}_count)
            node_dir="${output_dir}/${ip}/node${node_count}"
            mkdir -p "${node_dir}"
            generate_node_cert "${sm_mode}" "${ca_cert_dir}" "${node_dir}"
            account_dir="${node_dir}"

            local port=$((p2p_listen_port + node_count))
            connected_nodes=${connected_nodes}"${ip}:${port}, "

            if [[ "${sm_mode}" == "false" ]]; then
                generate_node_account "${account_dir}" "${count}"
            else
                generate_sm_node_account "${account_dir}" "${count}"
            fi
            set_value ${ip//./}_count $(( $(get_value ${ip//./}_count) + 1 ))
            ((++count))
            ((++node_count))
        done
    done

    local i=0
    local count=0
    for line in ${ip_array[*]};do
        ip=${line%:*}
        num=${line#*:}
        if [ -z $(echo $ip | grep -E "^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$") ];then
            LOG_WARN "Please check IP address: ${ip}, if you use domain name please ignore this."
        fi
        [ "$num" == "$ip" ] || [ -z "${num}" ] && num=${node_num} 
        set_value ${ip//./}_count 0
        for ((i=0;i<num;++i));do
            local node_count=$(get_value ${ip//./}_count)
            node_dir="${output_dir}/${ip}/node${node_count}"
            local p2p_port=$((p2p_listen_port + node_count))
            local rpc_port=$((rpc_listen_port + node_count))
            generate_config "${sm_mode}" "${node_dir}/config.ini" "${node_dir}" "${connected_nodes}" "${p2p_port}" "${rpc_port}"
            generate_genesis_config "${node_dir}/config.genesis" "${nodeid_list}"
            set_value ${ip//./}_count $(( $(get_value ${ip//./}_count) + 1 ))
            ((++count))
        done
    done
    print_result
}

main "$@"
