#pragma once

namespace wfc{ namespace io{ namespace reader{

struct _start_;
struct _is_started_;

// Причитать еще 
struct _read_more_;
struct _on_read_;

struct _error_code_;
// Опбработчик результата чтнения 
struct _read_handler_;
// Чтение успешно _read_handler_->_read_ready_
struct _read_ready_;
// Чтение прервано _read_handler_->_read_aborted_
struct _read_aborted_;
// Ошибка чтения _read_handler_->_read_error_
struct _read_error_;
// Создание буффера для чтения
struct _make_buffer_;
// Чтение из сокета
struct _read_some_;

struct _on_abort_;
struct _on_error_;
struct _free_buffer_;
struct _handler_;
struct _user_handler_;
// Глобально, для всей цепочки например reader->rm->user->_outgoing_
struct _outgoing_;
struct _incoming_;
struct _output_;



}}}
