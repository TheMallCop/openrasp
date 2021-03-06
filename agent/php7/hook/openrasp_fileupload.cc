/*
 * Copyright 2017-2018 Baidu Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "openrasp_hook.h"

/**
 * fileupload相关hook点
 */
PRE_HOOK_FUNCTION(move_uploaded_file, fileUpload);

void pre_global_move_uploaded_file_fileUpload(OPENRASP_INTERNAL_FUNCTION_PARAMETERS)
{
    zval *name, *dest;
    int argc = MIN(2, ZEND_NUM_ARGS());

    if (argc < 2 ||
        zend_get_parameters_ex(argc, &name, &dest) != SUCCESS ||
        Z_TYPE_P(name) != IS_STRING ||
        Z_TYPE_P(dest) != IS_STRING ||
        !zend_hash_exists(SG(rfc1867_uploaded_files), Z_STR_P(name)) ||
        php_check_open_basedir_ex(Z_STRVAL_P(dest), 0) ||
        (Z_TYPE(PG(http_globals)[TRACK_VARS_FILES]) != IS_STRING && !zend_is_auto_global_str(ZEND_STRL("_FILES"))))
    {
        return;
    }

    zval *realname = nullptr, *file;
    ZEND_HASH_FOREACH_VAL(Z_ARRVAL(PG(http_globals)[TRACK_VARS_FILES]), file)
    {
        zval *tmp_name = NULL;
        if (Z_TYPE_P(file) != IS_ARRAY ||
            (tmp_name = zend_hash_str_find(Z_ARRVAL_P(file), ZEND_STRL("tmp_name"))) == NULL ||
            Z_TYPE_P(tmp_name) != IS_STRING ||
            zend_binary_strcmp(Z_STRVAL_P(tmp_name), Z_STRLEN_P(tmp_name), Z_STRVAL_P(name), Z_STRLEN_P(name)) != 0)
        {
            continue;
        }
        if ((realname = zend_hash_str_find(Z_ARRVAL_P(file), ZEND_STRL("name"))) != NULL)
        {
            break;
        }
    }
    ZEND_HASH_FOREACH_END();
    if (!realname)
    {
        realname = dest;
    }
    php_stream *stream = php_stream_open_wrapper(Z_STRVAL_P(name), "rb", 0, NULL);
    if (!stream)
    {
        return;
    }
    zend_string *buffer = php_stream_copy_to_mem(stream, 4 * 1024, 0);
    stream->is_persistent ? php_stream_pclose(stream) : php_stream_close(stream);
    if (!buffer)
    {
        return;
    }
    zval params;
    array_init(&params);
    add_assoc_zval(&params, "filename", realname);
    Z_ADDREF_P(realname);
    add_assoc_str(&params, "content", buffer);
    check(check_type, &params);
}