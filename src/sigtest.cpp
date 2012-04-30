/*
 * Glib::Dispatcher example -- cross thread signalling
 * by Daniel Elstner <daniel.kitta@gmail.com>
 *
 * modified to only use glibmm
 * by J. Abelardo Gutierrez <jabelardo@cantv.net>
 *
 * fixed for glibmm-2.4, nb autoformat
 * by Trevor Bergeron <mallegonian@gmail.com>
 *
 * Copyright (c) 2002-2003 Free Software Foundation
 *
 * From http://developer.gnome.org/glibmm/unstable/classGlib_1_1Dispatcher.html
 *
 */

#include "common.h"

// for thread return stuff, maybe not all needed
#include <glibmm.h>
#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>

class wait_data2 {
public:
    explicit wait_data2(int idn);
    virtual ~wait_data2();

    int id;
    void launch();
    void join();
    bool unfinished() const;

    sigc::signal<void>& signal_update_board();

    int data;
private:

    enum {
        ITERATIONS = 3
    };

    // Note that the thread does not write to the member data at all. It only
    // reads dispatcher, which is only written to before the thread is
    // launched. Therefore, no locking is required.
    Glib::Thread::Thread * thread_;
    unsigned int progress_;
    Glib::Dispatcher dispatcher;
    sigc::signal<void> sig_fin;

    void progress_increment();
    void thread_function();
};

class BPwin2 : public sigc::trackable {
public:
    BPwin2();

    void run();

private:
    Glib::RefPtr<Glib::MainLoop> main_loop_;
    std::vector<wait_data2*> progress_thread;

    void launch_threads();
    void on_progress_finished(wait_data2* thread_progress);
};

wait_data2::wait_data2(int idn)
:
thread_(0),
id(idn),
progress_(0) {
    // Connect to the cross-thread signal.
    dispatcher.connect(sigc::mem_fun(*this, &wait_data2::progress_increment));
}

wait_data2::~wait_data2() {
    // It is an error if the thread is still running at this point.
    g_return_if_fail(thread_ == 0);
}

void wait_data2::launch() {
    // Create a joinable thread.
    thread_ = Glib::Thread::Thread::create(sigc::mem_fun(*this, &wait_data2::thread_function), true);
}

void wait_data2::join() {
    thread_->join();
    thread_ = 0;
}

bool wait_data2::unfinished() const {
    return (progress_ < ITERATIONS);
}

sigc::signal<void>& wait_data2::signal_update_board() {
    return sig_fin;
}

void wait_data2::progress_increment() {
    ++progress_;
    std::cout << "Thread " << id << ": " << progress_ << std::endl;

    if (progress_ >= ITERATIONS)
        sig_fin();
}

void wait_data2::thread_function() {
    Glib::Rand rand;

    for (int i = 0; i < ITERATIONS; ++i) {
        Glib::usleep(rand.get_int_range(2000, 20000));

        // Tell the main thread to increment the progress value.
        dispatcher();
    }
}

BPwin2::BPwin2()
:
main_loop_(Glib::MainLoop::create()) {
    for (std::vector<wait_data2*>::size_type i = 0; i < progress_thread.size(); ++i) {
        wait_data2 * const progress = new wait_data2(i + 1);
        progress_thread = progress;

        progress->signal_update_board().connect(
                sigc::bind < 1 > (sigc::mem_fun(*this, &BPwin2::on_progress_finished), progress));
    }
}

void BPwin2::run() {
    // Install a one-shot idle handler to launch the threads.
    Glib::signal_idle().connect_once(sigc::mem_fun(*this, &BPwin2::launch_threads));

    main_loop_->run();
}

void BPwin2::launch_threads() {
    std::cout << "Launching " << progress_thread.size() << " threads:" << std::endl;

    std::for_each(progress_thread.begin(), progress_thread.end(),
            std::mem_fun(&wait_data2::launch));
}

void BPwin2::on_progress_finished(wait_data2* thread_progress) {
    thread_progress->join();

    std::cout << "Thread " << thread_progress->id() << ": finished." << std::endl;

    // Quit if it was the last thread to be joined.
    if (std::find_if(progress_thread.begin(), progress_thread.end(),
            std::mem_fun(&wait_data2::unfinished)) == progress_thread.end()) {
        main_loop_->quit();
    }
}

int main(int argc, char** argv) {
    Glib::init();

    BPwin2 BPwin2;
    BPwin2.run();

    return 0;
}
